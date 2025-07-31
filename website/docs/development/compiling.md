---
sidebar_position: 2
---

# Compiling the Software

:::info Prerequisite
Make sure the [Toolchain is set up](./toolchain.md) as described.
:::

---

## Adding a Build Configuration

1. **Open VS Code** with the nRF Connect extension.
2. In the **nRF Connect** sidebar, under **Application**, select <code>app</code>.
3. Click <strong>Add build configuration</strong>.

   - **Target:**  
     Select the board or target you want to compile for.

   - **Extra Kconfig fragments:**  
     Add any required Kconfig fragments.  
     <details>
       <summary>Example for builds with debug logs (where <code>x</code> is <code>rtt</code>, <code>uart</code>, or <code>usb</code>):</summary>

       ```
       debug.conf, log_on_x.conf
       ```
     </details>

   - **Extra Devicetree overlays:**  
     Add any required overlay files.  
     <details>
       <summary>Example:</summary>

       ```
       log_on_x.overlay
       ```
     </details>

   - **Build Directory (optional):**  
     You can name the build directory, e.g. <code>build_devkit</code> for the ZSWatch devkit.

4. Click <strong>Generate and Build</strong>.

---

:::success
**Done!** Your firmware will be compiled with the selected configuration.
:::

:::tip
Under <b>Actions</b> in the nRF Connect plugin, you can quickly <b>rebuild</b>, <b>debug</b>, <b>flash</b>, and perform other tasks on your firmware.