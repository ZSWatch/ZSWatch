SET(SOURCES 
    screens/notification_ui.c
    images/ui_img_trash_png.c
    images/ui_img_call_png.c
    images/ui_img_mail_png.c
    images/ui_img_gadget_png.c
    images/ui_img_apple.c
    images/ui_img_youtube_png.c
    images/ui_img_homeassistant_png.c
    images/ui_img_whatsapp_png.c
    images/ui_img_discord_png.c
    images/ui_img_linkedin_png.c)

target_sources(app PRIVATE ${SOURCES})