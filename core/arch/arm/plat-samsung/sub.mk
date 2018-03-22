global-incdirs-y += .
srcs-y += main.c

srcs-$(PLATFORM_FLAVOR_artik520) += a7_plat_init.S
srcs-$(PLATFORM_FLAVOR_artik530) += a9_plat_init.S