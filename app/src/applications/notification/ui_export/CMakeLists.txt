SET(SOURCES 
    screens/notification_ui.c
    images/ui_img_call_png.c
    images/ui_img_mail_png.c
    images/ui_img_gadget_png.c
    images/ui_img_whatsapp_png.c)

target_sources(app PRIVATE ${SOURCES})