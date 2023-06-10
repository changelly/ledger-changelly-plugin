#include "changelly_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
    ethQueryContractID_t *msg = (ethQueryContractID_t *) parameters;
    const context_t *context = (const context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    switch (context->selectorIndex) {
        case TRANSFORM_ERC20:
        case SAIL_ADAPTER_SWAP:
        case SAIL_ADAPTER_SWAP_WITH_FEE:
        case SELL_TO_UNISWAP:
        case SELL_TO_LIQUIDITY_PROVIDER:
        case SELL_ETH_FOR_TOKEN_TO_UNISWAP_V3:
        case SELL_TOKEN_FOR_ETH_TO_UNISWAP_V3:
        case SELL_TOKEN_FOR_TOKEN_TO_UNISWAP_V3:
        case SELL_TO_PANCAKESWAP:
        case ANY_RECIPIENT_TRANSFORM_ERC20:
            strlcpy(msg->version, "Defi Swap", msg->versionLength);
            break;
        case SAIL_UNISWAP_V3_SWAP:
        case SAIL_UNISWAP_V3_SWAP_WITH_FEE:
        case SAIL_UNISWAP_V3_SWAP_WITH_SLIPPAGE:
        case SAIL_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE:
            strlcpy(msg->version, "Defi Swap UniswapV3", msg->versionLength);
            break;
        case SAIL_SPLIT_UNISWAP_V3_SWAP:
        case SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE:
        case SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_SLIPPAGE:
        case SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE:
            strlcpy(msg->version, "Defi Swap Split UniswapV3", msg->versionLength);
            break;
        default:
            PRINTF("Selector Index :%d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}