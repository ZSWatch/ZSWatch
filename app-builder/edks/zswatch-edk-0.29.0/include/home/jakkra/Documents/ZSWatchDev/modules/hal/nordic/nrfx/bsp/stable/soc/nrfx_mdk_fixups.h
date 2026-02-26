#ifndef NRFX_MDK_FIXUPS_H__
#define NRFX_MDK_FIXUPS_H__

/**************************************************************************************************/
/* Start fixups section for NRF51                                                                 */
/**************************************************************************************************/

#if defined(NRF51)
    #define NVMC_FLASH_BASE_ADDRESS 0

    #define ECB_TASKS_STARTECB_TASKS_STARTECB_Msk
    #define ECB_TASKS_STOPECB_TASKS_STOPECB_Msk
    #define ECB_EVENTS_ENDECB_EVENTS_ENDECB_Msk
    #define ECB_EVENTS_ERRORECB_EVENTS_ERRORECB_Msk
    #define ECB_ECBDATAPTR_ECBDATAPTR_Msk

    #define EVENT_READBACK_NOT_NEEDED 1

    #define FICR_CODEPAGESIZE_CODEPAGESIZE_Msk
    #define FICR_CODESIZE_CODESIZE_Msk
    #define FICR_DEVICEID_DEVICEID_Msk
    #define FICR_DEVICEADDR_DEVICEADDR_Msk
    #define FICR_ER_ER_Msk
    #define FICR_IR_IR_Msk

    #define UICR_CLENR0_CLENR0_Msk

    #define POWER_TASKS_CONSTLAT_TASKS_CONSTLAT_Msk
    #define POWER_TASKS_LOWPWR_TASKS_LOWPWR_Msk

    #define QDEC_LEDPRE_ResetValue 0x10

    #define WDT_RR_MaxCount 8
    #define CCM_TASKS_KSGEN_TASKS_KSGEN_Msk
    #define CCM_TASKS_CRYPT_TASKS_CRYPT_Msk

    #define AAR_IRKPTR_IRKPTR_Msk
    #define AAR_ADDRPTR_ADDRPTR_Msk
    #define AAR_SCRATCHPTR_SCRATCHPTR_Msk

    #define DELAY_CUSTOM_CYCLES 4
    #define DWT_MISSING
#endif

/**************************************************************************************************/
/* End fixups section for NRF51                                                                   */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF52_SERIES                                                          */
/**************************************************************************************************/
#if defined(NRF52_SERIES)
    #define QDEC_LEDPRE_ResetValue           0x10
    #define QSPI_BASE_CLOCK_FREQ             32000000uL
    #define SPIM_PSEL_DCX_ResetValue         0xFFFFFFFFUL
    #define SPIM_PSEL_CSN_ResetValue         0xFFFFFFFFUL
    #define SPIM_IFTIMING_CSNDUR_ResetValue  0x2UL
    #define SPIM_CSNPOL_ResetValue           0x0UL
    #define SPIM_IFTIMING_RXDELAY_ResetValue 0x2UL
    #define WDT_RR_MaxCount 8
    #define SAADC_SAMPLERATE_CC_Min          80UL
    #define SAADC_SAMPLERATE_CC_Max          2047UL
    #define NFCT_FRAMEDELAYMIN_ResetValue    0x480UL
    #define NFCT_FRAMEDELAYMAX_ResetValue    0x1000UL
    #define NVMC_FLASH_BASE_ADDRESS          0
    #if defined(NRF52805_XXAA) || defined(NRF52810_XXAA) || \
        defined(NRF52811_XXAA) || defined(NRF52840_XXAA)
        #define NVMC_PAGE_ERASE_DURATION_MS  85
    #elif defined(NRF52820_XXAA) || defined(NRF52833_XXAA)
        #define NVMC_PAGE_ERASE_DURATION_MS  87
    #endif
    #if defined(NRF52810_XXAA) || defined(NRF52811_XXAA) || \
        defined(NRF52832_XXAA) || defined(NRF52834_XXAA)
        #define PDM_RATIO_VALUE 64
    #endif
    #if defined(NRF52805_XXAA) || defined(NRF52810_XXAA) || \
        defined(NRF52811_XXAA) || defined(NRF52820_XXAA)
        #define DELAY_CUSTOM_CYCLES 7
        #define DWT_MISSING
    #endif
#endif
/**************************************************************************************************/
/* End fixups section for NRF52_SERIES                                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF52820_XXAA                                                         */
/**************************************************************************************************/
#if defined(NRF52820_XXAA)
    #if defined(DEVELOP_IN_NRF52833)
    /* Allow use of the following additional GPIOs that are connected to LEDs and buttons
    * on the nRF52833 DK:
    * - P0.11 - Button 1
    * - P0.12 - Button 2
    * - P0.13 - LED 1
    * - P0.24 - Button 3
    * - P0.25 - Button 4
    */
        #define GPIO_PINS_OVERRIDE 0x03003800
    #endif
    #define GPIOTE_SPARSE_PINS
#endif
/**************************************************************************************************/
/* End fixups section for NRF52820_XXAA                                                           */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF53_SERIES                                                          */
/**************************************************************************************************/
#if defined(NRF53_SERIES)
    #define POWER_GPREGRET_MaxCount 2
    #define QDEC_LEDPRE_ResetValue 0x10
    #define QSPI_BASE_CLOCK_FREQ    96000000uL
    #define SPIM_PSEL_DCX_ResetValue         0xFFFFFFFFUL
    #define SPIM_PSEL_CSN_ResetValue         0xFFFFFFFFUL
    #define SPIM_IFTIMING_CSNDUR_ResetValue  0x2UL
    #define SPIM_CSNPOL_ResetValue           0x0UL
    #define SPIM_IFTIMING_RXDELAY_ResetValue 0x2UL
    #define SPIM_DCX_DISCONNECTED_READBACK   0x11F
    #if defined(NRF_APPLICATION)
        #define VMC_RAM_SECTION_COUNT   16
        #define NVMC_FLASH_BASE_ADDRESS 0
        #define NVMC_FLASH_PAGE_COUNT   256
        #define NVMC_FLASH_PAGE_SIZE    0x1000 ///< 4 kB
        #if defined(NRF_TRUSTZONE_NONSECURE)
            #define GPIOTE_ASSERT_INSTANCE  NRF_GPIOTE1
        #else
            #define GPIOTE_ASSERT_INSTANCE  NRF_GPIOTE0
        #endif
    #else
        #define VMC_RAM_SECTION_COUNT   4
        #define NVMC_FLASH_BASE_ADDRESS 0x01000000UL
        #define NVMC_FLASH_PAGE_COUNT   128
        #define NVMC_FLASH_PAGE_SIZE    0x800  ///< 2 kB
        #undef CLOCK_INTENSET_HFCLK192MSTARTED_Msk
        #undef CLOCK_INTENSET_HFCLKAUDIOSTARTED_Msk
        #undef CLOCK_HFCLKCTRL_HCLK_Div1
    #endif
    #define WDT_RR_MaxCount 8
    #define SAADC_SAMPLERATE_CC_Min          80UL
    #define SAADC_SAMPLERATE_CC_Max          2047UL
    #define NFCT_FRAMEDELAYMIN_ResetValue    0x480UL
    #define NFCT_FRAMEDELAYMAX_ResetValue    0x1000UL
    #define NVMC_PAGE_ERASE_DURATION_MS      87
    #define TWIM_1MBPS_NEEDS_E0E1

    #define QSPI_IO0_DEDICATED NRF_GPIO_PIN_MAP(0, 13)
    #define QSPI_IO1_DEDICATED NRF_GPIO_PIN_MAP(0, 14)
    #define QSPI_IO2_DEDICATED NRF_GPIO_PIN_MAP(0, 15)
    #define QSPI_IO3_DEDICATED NRF_GPIO_PIN_MAP(0, 16)
    #define QSPI_SCK_DEDICATED NRF_GPIO_PIN_MAP(0, 17)
    #define QSPI_CSN_DEDICATED NRF_GPIO_PIN_MAP(0, 18)

    #if defined(NRF_APPLICATION)
        #define SPIM_SCK_DEDICATED  NRF_GPIO_PIN_MAP(0, 8)
        #define SPIM_MOSI_DEDICATED NRF_GPIO_PIN_MAP(0, 9)
        #define SPIM_MISO_DEDICATED NRF_GPIO_PIN_MAP(0, 10)
        #define SPIM_CSN_DEDICATED  NRF_GPIO_PIN_MAP(0, 11)
        #define SPIM_DCX_DEDICATED  NRF_GPIO_PIN_MAP(0, 12)
    #endif

    #define DPPI_TYPE_NORMAL

    #define PDM_PDMCLKCTRL_FACTOR 4096
    #define PDM_FREQ_FACTOR       1048576ULL

    #define DOMAIN_NET

    #define I2S_MCKFREQ_FACTOR 1048576
#endif
/**************************************************************************************************/
/* End fixups section for NRF53_SERIES                                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF91_SERIES                                                          */
/**************************************************************************************************/
#if defined(NRF91_SERIES)
    #define POWER_GPREGRET_MaxCount          2L
    #define SPIM_PSEL_DCX_ResetValue         0xFFFFFFFFUL
    #define SPIM_PSEL_CSN_ResetValue         0xFFFFFFFFUL
    #define SPIM_IFTIMING_CSNDUR_ResetValue  0x2UL
    #define SPIM_CSNPOL_ResetValue           0x0UL
    #define SPIM_IFTIMING_RXDELAY_ResetValue 0x2UL
    #define VMC_RAM_SECTION_COUNT            4
    #define VMC_FEATURE_RAM_REGISTERS_COUNT  4
    #define WDT_RR_MaxCount 8
    #define SAADC_SAMPLERATE_CC_Min          80UL
    #define SAADC_SAMPLERATE_CC_Max          2047UL
    #define NVMC_FLASH_BASE_ADDRESS          0
    #define NVMC_FLASH_PAGE_COUNT            256
    #define NVMC_FLASH_PAGE_SIZE             0x1000 ///< 4 kB
    #define NVMC_PAGE_ERASE_DURATION_MS      87
    #if defined(NRF_TRUSTZONE_NONSECURE)
        #define GPIOTE_ASSERT_INSTANCE  NRF_GPIOTE1
    #else
        #define GPIOTE_ASSERT_INSTANCE  NRF_GPIOTE0
    #endif

    #define DPPI_TYPE_NORMAL

    #define DOMAIN_MODEM
#endif
/**************************************************************************************************/
/* End fixups section for NRF91_SERIES                                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for HALTIUM_XXAA                                                          */
/**************************************************************************************************/

#if defined(HALTIUM_XXAA)
    #include "mdk/haltium_interim.h"

    #define EASYVDMA_PRESENT

    typedef struct {
        __IOM uint32_t  STMSTIMR[32];
        __IM  uint32_t  RESERVED0[737];
        __OM  uint32_t  STMDMASTARTR;
        __OM  uint32_t  STMDMASTOPR;
        __IM  uint32_t  STMDMASTATR;
        __IOM uint32_t  STMDMACTLR;
        __IM  uint32_t  RESERVED1[58];
        __IM  uint32_t  STMDMAIDR;
        __IOM uint32_t  STMHEER;
        __IM  uint32_t  RESERVED2[7];
        __IOM uint32_t  STMHETER;
        __IM  uint32_t  RESERVED3[16];
        __IOM uint32_t  STMHEMCR;
        __IM  uint32_t  RESERVED4[35];
        __IM  uint32_t  STMHEMASTR;
        __IM  uint32_t  STMHEFEAT1R;
        __IM  uint32_t  STMHEIDR;
        __IOM uint32_t  STMSPER;
        __IM  uint32_t  RESERVED5[7];
        __IOM uint32_t  STMSPTER;
        __IM  uint32_t  RESERVED6[15];
        __IOM uint32_t  STMSPSCR;
        __IOM uint32_t  STMSPMSCR;
        __IOM uint32_t  STMSPOVERRIDER;
        __IOM uint32_t  STMSPMOVERRIDER;
        __IOM uint32_t  STMSPTRIGCSR;
        __IM  uint32_t  RESERVED7[3];
        __IOM uint32_t  STMTCSR;
        __OM  uint32_t  STMTSSTIMR;
        __IM  uint32_t  RESERVED8;
        __IOM uint32_t  STMTSFREQR;
        __IOM uint32_t  STMSYNCR;
        __IOM uint32_t  STMAUXCR;
        __IM  uint32_t  RESERVED9[2];
        __IOM uint32_t  STMSPFEAT1R;
        __IOM uint32_t  STMSPFEAT2R;
        __IOM uint32_t  STMSPFEAT3R;
        __IM  uint32_t  RESERVED10[15];
        __OM  uint32_t  STMITTRIGGER;
        __OM  uint32_t  STMITATBDATA0;
        __OM  uint32_t  STMITATBCTR2;
        __OM  uint32_t  STMITATBID;
        __OM  uint32_t  STMITATBCTR0;
        __IM  uint32_t  RESERVED11;
        __IOM uint32_t  ITCTRL;
        __IM  uint32_t  RESERVED12[43];
        __IOM uint32_t  LAR;
        __IOM uint32_t  LSR;
        __IOM uint32_t  AUTHSTATUS;
        __IM  uint32_t  RESERVED13[3];
        __IM  uint32_t  DEVID;
        __IM  uint32_t  DEVTYPE;
        __IOM uint32_t  PIDR4;
        __IM  uint32_t  RESERVED14[3];
        __IOM uint32_t  PIDR0;
        __IOM uint32_t  PIDR1;
        __IOM uint32_t  PIDR2;
        __IOM uint32_t  PIDR3;
        __IOM uint32_t  CIDR0;
        __IOM uint32_t  CIDR1;
        __IOM uint32_t  CIDR2;
        __IOM uint32_t  CIDR3;
    } NRF_STM_Type_fixed;

    #if defined(NRF_STM_NS)
        #undef NRF_STM_NS
        #define NRF_STM_NS ((NRF_STM_Type_fixed*) NRF_STM_NS_BASE)
    #endif

    #define NRF_STM_Type NRF_STM_Type_fixed

    #if defined(NRF_ETB_NS)
        #undef NRF_ETB_NS
        #define NRF_ETB_NS ((NRF_TMC_Type*) NRF_ETB_NS_BASE)
    #endif

    typedef struct {
        __OM  uint32_t G_DMTS[2];
        __OM  uint32_t G_DM[2];
        __OM  uint32_t G_DTS[2];
        __OM  uint32_t G_D[2];
        __IM  uint32_t RESERVED0[16];
        __OM  uint32_t G_FLAGTS[2];
        __OM  uint32_t G_FLAG[2];
        __OM  uint32_t G_TRIGTS[2];
        __OM  uint32_t G_TRIG[2];
        __OM  uint32_t I_DMTS[2];
        __OM  uint32_t I_DM[2];
        __OM  uint32_t I_DTS[2];
        __OM  uint32_t I_D[2];
        __IM  uint32_t RESERVED1[16];
        __OM  uint32_t I_FLAGTS[2];
        __OM  uint32_t I_FLAG[2];
        __OM  uint32_t I_TRIGTS[2];
        __OM  uint32_t I_TRIG[2];
    } NRF_STMESP_Type;

    #define NRF_APPLICATION_STMESP_NS_BASE 0xA2000000UL
    #define NRF_APPLICATION_STMESP_NS      ((NRF_STMESP_Type*) NRF_APPLICATION_STMESP_NS_BASE)
    #define NRF_APPLICATION_STMESP         NRF_APPLICATION_STMESP_NS

    #define NRF_RADIOCORE_STMESP_NS_BASE 0xA3000000UL
    #define NRF_RADIOCORE_STMESP_NS      ((NRF_STMESP_Type*) NRF_RADIOCORE_STMESP_NS_BASE)
    #define NRF_RADIOCORE_STMESP         NRF_RADIOCORE_STMESP_NS

    #if defined(NRF_APPLICATION)
        #define NRF_STMESP NRF_APPLICATION_STMESP
    #endif

    #if defined(NRF_RADIOCORE)
        #define NRF_STMESP NRF_RADIOCORE_STMESP
    #endif

    #define DMA_BUFFER_UNIFIED_BYTE_ACCESS 1

    #define TYPES_DOMAIN
    #define TYPES_PROCESSOR
    #define TYPES_OWNER

    #define DMA_ACCESSIBLE_CUSTOM_CHECK 1

    #if defined(NRF_APPLICATION) || defined(NRF_PPR) || defined(NRF_FLPR)
        #define GPIOTE_PORT_ID 1
    #elif defined(NRF_RADIOCORE)
        #define GPIOTE_PORT_ID 2
    #endif

    #if (!defined(__VPR_REV) && defined(NRF_TRUSTZONE_NONSECURE)) || defined(__VPR_REV)
        #define GPIOTE130_IRQn       GPIOTE130_0_IRQn
        #define GPIOTE130_IRQHandler GPIOTE130_0_IRQHandler
    #else
        #define GPIOTE130_IRQn       GPIOTE130_1_IRQn
        #define GPIOTE130_IRQHandler GPIOTE130_1_IRQHandler
    #endif

    #if defined(__VPR_REV)
        #define GPIOTE_SECURE_SUFFIX_OVERRIDE NONSECURE
    #endif

    #if (!defined(__VPR_REV) && defined(NRF_TRUSTZONE_NONSECURE)) || defined(__VPR_REV)
        #define GRTC_IRQn       GRTC_0_IRQn
        #define GRTC_IRQHandler GRTC_0_IRQHandler
    #else
        #define GRTC_IRQn       GRTC_1_IRQn
        #define GRTC_IRQHandler GRTC_1_IRQHandler
    #endif

    #define GRTC_MAIN_CC_CHANNEL 1

    #define I2S_CLOCKPIN_SCK_NEEDED
    #define I2S_CLOCKPIN_LRCK_NEEDED
    #define I2S_CLOCKPIN_MCK_NEEDED

    #define SPIM_CLOCKPIN_MOSI_NEEDED
    #define SPIM_CLOCKPIN_SCK_NEEDED

    #define SPIS_CLOCKPIN_MISO_NEEDED
    #define SPIS_CLOCKPIN_SCK_NEEDED

    #define TDM_CLOCKPIN_SCK_NEEDED
    #define TDM_CLOCKPIN_FSYNC_NEEDED
    #define TDM_CLOCKPIN_MCK_NEEDED

    #define EXMIF_MAX_MEMORY_DEVICE_SIZE 0x10000000UL
    #define EXMIF_MAX_NUMBER_OF_DEVICES  2

    #define TWIM_CLOCKPIN_SCL_NEEDED
    #define TWIS_CLOCKPIN_SCL_NEEDED
    #define UARTE_CLOCKPIN_TXD_NEEDED

    #define SPIM_CHECK_DISABLE_ON_XFER_END

    #define DPPI_TYPE_IPCT

    #define DOMAIN_FLPR
#endif

/**************************************************************************************************/
/* End fixups section for HALTIUM_XXAA                                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for LUMOS_XXAA                                                            */
/**************************************************************************************************/

#if defined(LUMOS_XXAA)
    #define DMA_BUFFER_UNIFIED_BYTE_ACCESS 1

    #if (defined(NRF_APPLICATION) && defined(NRF_TRUSTZONE_NONSECURE)) || \
        defined(NRF_FLPR) || defined(NRF54LS05B_ENGA_XXAA)
        #define GPIOTE20_IRQn       GPIOTE20_0_IRQn
        #define GPIOTE20_IRQHandler GPIOTE20_0_IRQHandler
        #define GPIOTE30_IRQn       GPIOTE30_0_IRQn
        #define GPIOTE30_IRQHandler GPIOTE30_0_IRQHandler
    #else
        #define GPIOTE20_IRQn       GPIOTE20_1_IRQn
        #define GPIOTE20_IRQHandler GPIOTE20_1_IRQHandler
        #define GPIOTE30_IRQn       GPIOTE30_1_IRQn
        #define GPIOTE30_IRQHandler GPIOTE30_1_IRQHandler
    #endif

    #define GPIOTE_PORT_ID 0

    #if defined(NRF_TRUSTZONE_NONSECURE)
    /* Non-secure images must have CPU frequency specified and cannot rely on default values,
     * as NRF_OSCILLATORS might be assigned and configured by Secure image. */
        #if defined(NRF_CONFIG_CPU_FREQ_MHZ) && (NRF_CONFIG_CPU_FREQ_MHZ == 64)
            #define NRF_CPU_FREQ_IS_64MHZ 1
        #elif defined(NRF_CONFIG_CPU_FREQ_MHZ) && (NRF_CONFIG_CPU_FREQ_MHZ == 128)
            #define NRF_CPU_FREQ_IS_128MHZ 1
        #elif defined(NRF_CONFIG_CPU_FREQ_MHZ) && (NRF_CONFIG_CPU_FREQ_MHZ == 256)
            #define NRF_CPU_FREQ_IS_256MHZ 1
        #elif !defined(NRF_CONFIG_CPU_FREQ_MHZ)
            #error "MCU frequency not specified"
        #else
            #error "Invalid MCU frequency"
        #endif
    #else
        #if defined(NRF_SKIP_CLOCK_CONFIGURATION) || \
            (defined(NRF_CONFIG_CPU_FREQ_MHZ) && (NRF_CONFIG_CPU_FREQ_MHZ == 64))
            #define NRF_CPU_FREQ_IS_64MHZ 1
        #elif defined(NRF_CONFIG_CPU_FREQ_MHZ) && (NRF_CONFIG_CPU_FREQ_MHZ == 128)
            #define NRF_CPU_FREQ_IS_128MHZ 1
        #elif !defined(NRF_CONFIG_CPU_FREQ_MHZ)
            /* If clock configuration is not skipped and frequency not specified,
            * SystemInit() applies 128 MHz setting. */
            #define NRF_CPU_FREQ_IS_128MHZ 1
        #elif defined(NRF_CONFIG_CPU_FREQ_MHZ) && (NRF_CONFIG_CPU_FREQ_MHZ == 256)
            #define NRF_CPU_FREQ_IS_256MHZ 1
        #else
            #error "Invalid MCU frequency"
        #endif
    #endif

    #define TYPES_DOMAIN
    #define TYPES_OWNER

    #define GRTC_FORCE_EXTENDED
    #if defined(NRF_FLPR) || defined(NRF54LS05B_ENGA_XXAA)
        #define GRTC_IRQn       GRTC_0_IRQn
        #define GRTC_IRQHandler GRTC_0_IRQHandler
    #elif defined(NRF_APPLICATION) && defined(NRF_TRUSTZONE_NONSECURE)
        #define GRTC_IRQn       GRTC_1_IRQn
        #define GRTC_IRQHandler GRTC_1_IRQHandler
    #elif defined(NRF_APPLICATION) && !defined(NRF_TRUSTZONE_NONSECURE)
        #define GRTC_IRQn       GRTC_2_IRQn
        #define GRTC_IRQHandler GRTC_2_IRQHandler
    #endif

    #if defined(NRF_FLPR)
        #define GRTC_MAIN_CC_CHANNEL 4
    #else
        #define GRTC_MAIN_CC_CHANNEL 0
    #endif

    #define SPIM_FORCE_H0H1

    #define DPPI_TYPE_PPIB

    #define DELAY_RISCV_SLOWDOWN 15
#endif

/**************************************************************************************************/
/* End fixups section for LUMOS_XXAA                                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54H20_XXAA (LILIUM 1.1)                                            */
/**************************************************************************************************/

#if defined(NRF54H20_XXAA)
    #include "mdk/nrf54h20_interim.h"

    #define RESETHUB_HAS_NETWORK

    #define TWIM_CLOCKPIN_SDA_NEEDED
    #define TWIS_CLOCKPIN_SDA_NEEDED

    #define PDM_PDMCLKCTRL_FACTOR 8192
    #define PDM_FREQ_FACTOR       1048576ULL

    #define DWT_MISSING
#endif

/**************************************************************************************************/
/* End fixups section for NRF54H20_XXAA (LILIUM 1.1)                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54L05_XXAA                                                         */
/**************************************************************************************************/

#if defined(NRF54L05_XXAA)
    #include "mdk/nrf54l05_interim.h"
    #define CLOCK_STATIC_IRQ
    #define TRNG_CLK_DIV 0
    #define I2S_MCKFREQ_FACTOR 1048576
#endif

/**************************************************************************************************/
/* End fixups section for NRF54L05_XXAA                                                           */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54L10_XXAA                                                         */
/**************************************************************************************************/

#if defined(NRF54L10_XXAA)
    #include "mdk/nrf54l10_interim.h"
    #define CLOCK_STATIC_IRQ
    #define TRNG_CLK_DIV 0
    #define I2S_MCKFREQ_FACTOR 1048576
#endif

/**************************************************************************************************/
/* End fixups section for NRF54L10_XXAA                                                           */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54L15_XXAA                                                         */
/**************************************************************************************************/

#if defined(NRF54L15_XXAA)
    #include "mdk/nrf54l15_interim.h"
    #define CLOCK_STATIC_IRQ
    #define TRNG_CLK_DIV 0
    #define I2S_MCKFREQ_FACTOR 1048576
#endif

/**************************************************************************************************/
/* End fixups section for NRF54L15_XXAA                                                           */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54LM20A_ENGA_XXAA                                                  */
/**************************************************************************************************/

#if defined(NRF54LM20A_ENGA_XXAA)
    #include "mdk/nrf54lm20a_enga_interim.h"
    #define TRNG_CLK_DIV 1
    #define I2S_MCKFREQ_FACTOR 1048576
#endif

/**************************************************************************************************/
/* End fixups section for NRF54LM20A_ENGA_XXAA                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54LS05B_ENGA_XXAA                                                  */
/**************************************************************************************************/

#if defined(NRF54LS05B_ENGA_XXAA)
    #include "mdk/nrf54ls05b_enga_interim.h"
    #define GPIOTE_SECURE_SUFFIX_OVERRIDE NONSECURE
    #define TRNG_CLK_DIV 1
#endif

/**************************************************************************************************/
/* End fixups section for NRF54LS05B_ENGA_XXAA                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF54LV10A_ENGA_XXAA                                                  */
/**************************************************************************************************/

#if defined(NRF54LV10A_ENGA_XXAA)
    #define TRNG_CLK_DIV 1
#endif

/**************************************************************************************************/
/* End fixups section for NRF54LV10A_ENGA_XXAA                                                    */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF7120_ENGA_XXAA                                                         */
/**************************************************************************************************/

#if defined(NRF7120_ENGA_XXAA)
    #include "mdk/nrf7120_enga_interim.h"

    #define ADDRESS_BUS_Pos (18UL)
    #define ADDRESS_BUS_Msk (0x3FUL << ADDRESS_BUS_Pos)
#endif

/**************************************************************************************************/
/* End fixups section for NRF7120_ENGA_XXAA                                                           */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for NRF9230_ENGB_XXAA (HSOC 1.1)                                          */
/**************************************************************************************************/

#if defined(NRF9230_ENGB_XXAA)
    #if defined(NRF_APPLICATION)
        #define NRF_DOMAIN NRF_DOMAIN_APPLICATION
    #elif defined(NRF_RADIOCORE)
        #define NRF_DOMAIN NRF_DOMAIN_RADIOCORE
    #elif defined(NRF_FLPR)
        #define NRF_DOMAIN NRF_DOMAIN_GLOBALFAST
    #elif defined(NRF_PPR)
        #define NRF_DOMAIN NRF_DOMAIN_GLOBALSLOW
    #endif

    #if defined(NRF_APPLICATION)
        #define NRF_PROCESSOR NRF_PROCESSOR_APPLICATION
    #elif defined(NRF_RADIOCORE)
        #define NRF_PROCESSOR NRF_PROCESSOR_RADIOCORE
    #elif defined(NRF_FLPR)
        #define NRF_PROCESSOR NRF_PROCESSOR_FLPR
    #elif defined(NRF_PPR)
        #define NRF_PROCESSOR NRF_PROCESSOR_PPR
    #endif

    #if defined(NRF_APPLICATION)
        #define NRF_OWNER NRF_OWNER_APPLICATION
    #elif defined(NRF_RADIOCORE)
        #define NRF_OWNER NRF_OWNER_RADIOCORE
    #elif defined(NRF_FLPR) && !defined(NRF_OWNER)
        #define NRF_OWNER NRF_OWNER_APPLICATION
    #elif defined(NRF_PPR) && !defined(NRF_OWNER)
        #define NRF_OWNER NRF_OWNER_APPLICATION
    #endif

    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Pos (2UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Msk (0x1UL << PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Pos)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Min (0x0UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Max (0x1UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Disabled (0x0UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ0_START_Enabled (0x1UL)

    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Pos (3UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Msk (0x1UL << PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Pos)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Min (0x0UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Max (0x1UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Disabled (0x0UL)
    #define PWM_SHORTS_LOOPSDONE_DMA_SEQ1_START_Enabled (0x1UL)

    #include "mdk/nrf9230_engb_interim.h"

    // Old HFXO modes are not supported
    #ifdef BICR_HFXO_CONFIG_MODE_Pierce
        #undef BICR_HFXO_CONFIG_MODE_Pierce
    #endif
    #ifdef BICR_HFXO_CONFIG_MODE_PIXO
        #undef BICR_HFXO_CONFIG_MODE_PIXO
    #endif
    #ifdef BICR_HFXO_CONFIG_MODE_ExtSquare
        #undef BICR_HFXO_CONFIG_MODE_ExtSquare
    #endif
    #ifdef BICR_HFXO_CONFIG_MODE_Auto
        #undef BICR_HFXO_CONFIG_MODE_Auto
    #endif
    #define BICR_HFXO_CONFIG_MODE_Normal   (0x0UL)     /*!< HFXO Normal mode.                                                    */
    #define BICR_HFXO_CONFIG_MODE_TCXO     (0x1UL)     /*!< HFXO TCXO/bypass mode.                                               */
    #define BICR_HFXO_CONFIG_MODE_Crystal2 (0x2UL)     /*!< Reserved value.                                                      */
    #define BICR_HFXO_CONFIG_MODE_Crystal3 (0x3UL)     /*!< Reserved value.                                                      */
    #define BICR_HFXO_CONFIG_MODE_Crystal4 (0x4UL)     /*!< Reserved value.                                                      */
    #define BICR_HFXO_CONFIG_MODE_Crystal5 (0x5UL)     /*!< Reserved value.                                                      */
    #define BICR_HFXO_CONFIG_MODE_Crystal6 (0x6UL)     /*!< Reserved value.                                                      */

    #define DPPIC0_CH_NUM DPPIC0_CH_NUM_SIZE
    #define DPPIC1_CH_NUM DPPIC1_CH_NUM_SIZE

    #define DPPIC0_GROUP_NUM DPPIC0_GROUP_NUM_SIZE
    #define DPPIC1_GROUP_NUM DPPIC1_GROUP_NUM_SIZE

    #define RESETHUB_HAS_NETWORK
    #define RESETHUB_HAS_CELLULAR
    #define RESETHUB_HAS_ISIM

    #define DWT_MISSING
#endif /* defined(NRF9230_ENGB_XXAA) */

/**************************************************************************************************/
/* End fixups section for NRF9230_ENGB_XXAA (HSOC 1.1)                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/* Start fixups section for external                                                              */
/**************************************************************************************************/

#include "soc/nrfx_mdk_fixups_ext.h"

/**************************************************************************************************/
/* End fixups section for external                                                                */
/**************************************************************************************************/

#endif // NRFX_MDK_FIXUPS_H__
