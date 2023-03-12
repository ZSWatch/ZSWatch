#include <ble_transport.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ble_transport, LOG_LEVEL_DBG);

static struct ble_transport_cb* callbacks;

static ssize_t on_receive(struct bt_conn *conn,
			  const struct bt_gatt_attr *attr,
			  const void *buf,
			  uint16_t len,
			  uint16_t offset,
			  uint8_t flags)
{
	LOG_DBG("Received data, handle %d, conn %p",
		attr->handle, (void *)conn);

	if (callbacks->data_receive) {
		callbacks->data_receive(conn, buf, len);
}
	return len;
}

BT_GATT_SERVICE_DEFINE(nus_service,
BT_GATT_PRIMARY_SERVICE(BLE_TRANSPORT_UUID_SERVICE),
	BT_GATT_CHARACTERISTIC(BLE_TRANSPORT_UUID_TX,
			       BT_GATT_CHRC_NOTIFY,
			       //BT_GATT_PERM_READ_AUTHEN, TODO when pairing is re-added
			       BT_GATT_PERM_READ,
			       NULL, NULL, NULL),
	BT_GATT_CCC(NULL,
			       //BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN),
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BLE_TRANSPORT_UUID_RX,
			       BT_GATT_CHRC_WRITE |
			       BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			       //BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       NULL, on_receive, NULL),
);

int ble_transport_init(struct ble_transport_cb *callback)
{
	if (callback) {
		callbacks = callback;
	}

	return 0;
}

int ble_transport_send(struct bt_conn *connection, const uint8_t *data, uint16_t length)
{
	struct bt_gatt_notify_params params = {0};
	const struct bt_gatt_attr *attr = &nus_service.attrs[2];

	params.attr = attr;
	params.data = data;
	params.len = length;

	if (connection && bt_gatt_is_subscribed(connection, attr, BT_GATT_CCC_NOTIFY)) {
		return bt_gatt_notify_cb(connection, &params);
	} else {
		return -EINVAL;
	}
}
