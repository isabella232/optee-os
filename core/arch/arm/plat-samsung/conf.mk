# Log levels for the user-mode TAs
# Defines which messages are displayed on the secure console
# 0: none
# 1: error
# 2: error + warning
# 3: error + warning + debug
# 4: error + warning + debug + flow
# alter this as required
CFG_TEE_TA_LOG_LEVEL ?= 4

$(call force,CFG_ARM32_core,y)
$(call force,CFG_SECURE_TIME_SOURCE_REE,y)
$(call force,CFG_GENERIC_BOOT,y)
$(call force,CFG_PM_STUBS,y)
$(call force,CFG_WITH_ARM_TRUSTED_FW,n)
$(call force,CFG_GIC,y)
$(call force,CFG_CACHE_API,y)

CFG_BOOT_SYNC_CPU ?= n
CFG_BOOT_SECONDARY_REQUEST ?= n
CFG_CRYPTO_SIZE_OPTIMIZATION ?= n
CFG_WITH_STACK_CANARIES ?= y
CFG_WITH_STATS ?= y

# adding :
CFG_WITH_PAGER ?= n
#CFG_TEE_CORE_EMBED_INTERNAL_TESTS ?= y
#CFG_WITH_SOFTWARE_PRNG ?= n
CFG_ENABLE_SCTLR_RR ?= y

CFG_NUM_THREADS ?= 4

# turn off Spectre and Meltdown workarounds if necessary
# CFG_CORE_WORKAROUND_SPECTRE_BP ?= n

ifeq ($(PLATFORM_FLAVOR),artik520)
  # serial device is Samsung Exynos device
  $(call force,CFG_EXYNOS4210_UART,y)
  # non-secure entry address for U-Boot:
  CFG_NS_ENTRY_ADDR ?= 0x43E00000
  # we can not (yet) access the fdt blob in memory
  #CFG_DT_ADDR ?= 0x40800000
  #CFG_DT ?= y
  include core/arch/arm/cpu/cortex-a7.mk
endif

ifeq ($(PLATFORM_FLAVOR),artik530)
  # serial device is ARM PL011 device
  $(call force,CFG_PL011,y)
  # non-secure entry address for U-Boot:
  CFG_NS_ENTRY_ADDR ?= 0x94C00000
  # we can not (yet) access the fdt blob in memory
  #CFG_DT_ADDR ?= 0x9B000000
  #CFG_DT ?= y
  include core/arch/arm/cpu/cortex-a9.mk
endif

ta-targets = ta_arm32
