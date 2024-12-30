/**
 * @file qspi.cc
 * @author WittXie
 * @brief QSPI驱动
 * @version 0.1
 * @date 2024-11-25
 * 由于STM32QSPI有部分BUG，所以需要自己实现QSPI驱动
 * @copyright Copyright (c) 2024
 *
 */

/** @defgroup QSPI_Private_Constants QSPI Private Constants
 * @{
 */
#define QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE 0x00000000U                    /*!<Indirect write mode*/
#define QSPI_FUNCTIONAL_MODE_INDIRECT_READ ((uint32_t)QUADSPI_CCR_FMODE_0) /*!<Indirect read mode*/
#define QSPI_FUNCTIONAL_MODE_AUTO_POLLING ((uint32_t)QUADSPI_CCR_FMODE_1)  /*!<Automatic polling mode*/
#define QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED ((uint32_t)QUADSPI_CCR_FMODE)   /*!<Memory-mapped mode*/
/**
 * @}
 */

/* Private macro -------------------------------------------------------------*/
/** @defgroup QSPI_Private_Macros QSPI Private Macros
 * @{
 */
#define IS_QSPI_FUNCTIONAL_MODE(MODE) (((MODE) == QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE) || \
                                       ((MODE) == QSPI_FUNCTIONAL_MODE_INDIRECT_READ) ||  \
                                       ((MODE) == QSPI_FUNCTIONAL_MODE_AUTO_POLLING) ||   \
                                       ((MODE) == QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))

/**
 * @brief  Wait for a flag state until timeout.
 * @param  hqspi : QSPI handle
 * @param  Flag : Flag checked
 * @param  State : Value of the flag expected
 * @param  Tickstart : Tick start value
 * @param  Timeout : Duration of the timeout
 * @retval HAL status
 */
static HAL_StatusTypeDef QSPI_WaitFlagStateUntilTimeout(QSPI_HandleTypeDef *hqspi, uint32_t Flag,
                                                        FlagStatus State, uint32_t Tickstart, uint32_t Timeout)
{
    /* Wait until flag is in expected state */
    while ((__HAL_QSPI_GET_FLAG(hqspi, Flag)) != State)
    {
        /* Check for the Timeout */
        //    if (Timeout != HAL_MAX_DELAY)
        //    {
        //      if(((HAL_GetTick() - Tickstart) > Timeout) || (Timeout == 0U))
        //      {
        //        hqspi->State     = HAL_QSPI_STATE_ERROR;
        //        hqspi->ErrorCode |= HAL_QSPI_ERROR_TIMEOUT;

        //        return HAL_ERROR;
        //      }
        //    }
    }
    return HAL_OK;
}

/**
 * @brief  Configure the communication registers.
 * @param  hqspi : QSPI handle
 * @param  cmd : structure that contains the command configuration information
 * @param  FunctionalMode : functional mode to configured
 *           This parameter can be one of the following values:
 *            @arg QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE: Indirect write mode
 *            @arg QSPI_FUNCTIONAL_MODE_INDIRECT_READ: Indirect read mode
 *            @arg QSPI_FUNCTIONAL_MODE_AUTO_POLLING: Automatic polling mode
 *            @arg QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED: Memory-mapped mode
 * @retval None
 */
static void QSPI_Config(QSPI_HandleTypeDef *hqspi, QSPI_CommandTypeDef *cmd, uint32_t FunctionalMode)
{
    assert_param(IS_QSPI_FUNCTIONAL_MODE(FunctionalMode));

    if ((cmd->DataMode != QSPI_DATA_NONE) && (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))
    {
        /* Configure QSPI: DLR register with the number of data to read or write */
        WRITE_REG(hqspi->Instance->DLR, (cmd->NbData - 1U));
    }

    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE)
    {
        if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
        {
            /* Configure QSPI: ABR register with alternate bytes value */
            WRITE_REG(hqspi->Instance->ABR, cmd->AlternateBytes);

            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with instruction, address and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                                 cmd->AddressSize | cmd->AddressMode | cmd->InstructionMode |
                                                 cmd->Instruction | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(hqspi->Instance->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with instruction and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                                 cmd->AddressMode | cmd->InstructionMode |
                                                 cmd->Instruction | FunctionalMode));
            }
        }
        else
        {
            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with instruction and address ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateByteMode | cmd->AddressSize | cmd->AddressMode |
                                                 cmd->InstructionMode | cmd->Instruction | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(hqspi->Instance->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only instruction ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateByteMode | cmd->AddressMode |
                                                 cmd->InstructionMode | cmd->Instruction | FunctionalMode));
            }
        }
    }
    else
    {
        if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
        {
            /* Configure QSPI: ABR register with alternate bytes value */
            WRITE_REG(hqspi->Instance->ABR, cmd->AlternateBytes);

            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with address and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                                 cmd->AddressSize | cmd->AddressMode |
                                                 cmd->InstructionMode | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(hqspi->Instance->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                                 cmd->AddressMode | cmd->InstructionMode | FunctionalMode));
            }
        }
        else
        {
            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with only address ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                 cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                 cmd->AlternateByteMode | cmd->AddressSize |
                                                 cmd->AddressMode | cmd->InstructionMode | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(hqspi->Instance->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only data phase ----*/
                if (cmd->DataMode != QSPI_DATA_NONE)
                {
                    /* Configure QSPI: CCR register with all communications parameters */
                    WRITE_REG(hqspi->Instance->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                                     cmd->DataMode | (cmd->DummyCycles << QUADSPI_CCR_DCYC_Pos) |
                                                     cmd->AlternateByteMode | cmd->AddressMode |
                                                     cmd->InstructionMode | FunctionalMode));
                }
            }
        }
    }
}

/**
 * @brief  Configure the Memory Mapped mode.
 * @param  hqspi : QSPI handle
 * @param  cmd : structure that contains the command configuration information.
 * @param  cfg : structure that contains the memory mapped configuration information.
 * @note   This function is used only in Memory mapped Mode
 * @retval HAL status
 */
static HAL_StatusTypeDef QSPI_MemoryMapped(QSPI_HandleTypeDef *hqspi, QSPI_CommandTypeDef *cmd, QSPI_MemoryMappedTypeDef *cfg)
{
    HAL_StatusTypeDef status;
    uint32_t tickstart = HAL_GetTick();

    /* Check the parameters */
    assert_param(IS_QSPI_INSTRUCTION_MODE(cmd->InstructionMode));
    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE)
    {
        assert_param(IS_QSPI_INSTRUCTION(cmd->Instruction));
    }

    assert_param(IS_QSPI_ADDRESS_MODE(cmd->AddressMode));
    if (cmd->AddressMode != QSPI_ADDRESS_NONE)
    {
        assert_param(IS_QSPI_ADDRESS_SIZE(cmd->AddressSize));
    }

    assert_param(IS_QSPI_ALTERNATE_BYTES_MODE(cmd->AlternateByteMode));
    if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
    {
        assert_param(IS_QSPI_ALTERNATE_BYTES_SIZE(cmd->AlternateBytesSize));
    }

    assert_param(IS_QSPI_DUMMY_CYCLES(cmd->DummyCycles));
    assert_param(IS_QSPI_DATA_MODE(cmd->DataMode));

    assert_param(IS_QSPI_DDR_MODE(cmd->DdrMode));
    assert_param(IS_QSPI_DDR_HHC(cmd->DdrHoldHalfCycle));
    assert_param(IS_QSPI_SIOO_MODE(cmd->SIOOMode));

    assert_param(IS_QSPI_TIMEOUT_ACTIVATION(cfg->TimeOutActivation));

    /* Process locked */
    __HAL_LOCK(hqspi);

    if (hqspi->State == HAL_QSPI_STATE_READY)
    {
        hqspi->ErrorCode = HAL_QSPI_ERROR_NONE;

        /* Update state */
        hqspi->State = HAL_QSPI_STATE_BUSY_MEM_MAPPED;

        /* Wait till BUSY flag reset */
        status = QSPI_WaitFlagStateUntilTimeout(hqspi, QSPI_FLAG_BUSY, RESET, tickstart, hqspi->Timeout);

        if (status == HAL_OK)
        {
            /* Configure QSPI: CR register with timeout counter enable */
            MODIFY_REG(hqspi->Instance->CR, QUADSPI_CR_TCEN, cfg->TimeOutActivation);

            /* modified here */
            if (cmd->DdrMode == QSPI_DDR_MODE_ENABLE)
            {
                MODIFY_REG(hqspi->Instance->CR, QUADSPI_CR_SSHIFT, 0);
            }
            /* modified end */

            if (cfg->TimeOutActivation == QSPI_TIMEOUT_COUNTER_ENABLE)
            {
                assert_param(IS_QSPI_TIMEOUT_PERIOD(cfg->TimeOutPeriod));

                /* Configure QSPI: LPTR register with the low-power timeout value */
                WRITE_REG(hqspi->Instance->LPTR, cfg->TimeOutPeriod);

                /* Clear interrupt */
                __HAL_QSPI_CLEAR_FLAG(hqspi, QSPI_FLAG_TO);

                /* Enable the QSPI TimeOut Interrupt */
                __HAL_QSPI_ENABLE_IT(hqspi, QSPI_IT_TO);
            }

            /* Call the configuration function */
            QSPI_Config(hqspi, cmd, QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED);
        }
    }
    else
    {
        status = HAL_BUSY;
    }

    /* Process unlocked */
    __HAL_UNLOCK(hqspi);

    /* Return function status */
    return status;
}
