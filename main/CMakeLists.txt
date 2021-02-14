set(srcs "app_main.c"
        "i2c.c"
        "mqtt.c"
        "provisionamiento.c"
        "server.c"
        "deep.c"
        "tareas.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS  "."
                    EMBED_TXTFILES "certs/cacert.pem"
                                   "certs/prvtkey.pem")
