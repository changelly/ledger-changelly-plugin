/*******************************************************************************
 *   Ethereum 2 Deposit Application
 *   (c) 2020 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "os.h"
#include "cx.h"

#include "glyphs.h"

#include "changelly_plugin.h"

// NATIVE TOKEN ADDRESS
const uint8_t NATIVE_TOKEN_ADDRESS[ADDRESS_LENGTH] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                                                      0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
                                                      0xee, 0xee, 0xee, 0xee, 0xee, 0xee};

// Selector: sellToUniswap(address[],uint256,uint256,bool)
// Full signature: function sellToUniswap(IERC20TokenV06[] calldata tokens, uint256
// sellAmount,uint256 minBuyAmount,bool isSushi)
static const uint32_t SELL_TO_UNISWAP_SELECTOR = 0xd9627aa4;
// Selector: sellToLiquidityProvider(address,address,address,address,uint256,uint256,bytes)
// Full signature: function sellToLiquidityProvider(IERC20TokenV06 inputToken,IERC20TokenV06
// outputToken,ILiquidityProvider provider,address recipient,uint256 sellAmount,uint256
// minBuyAmount,bytes calldata auxiliaryData)
static const uint32_t SELL_TO_LIQUIDITY_PROVIDER_SELECTOR = 0xf7fcd384;
// Selector: transformERC20(address,address,uint256,uint256,(uint32,bytes)[])
// Full Signature: function transformERC20(IERC20TokenV06 inputToken,IERC20TokenV06
// outputToken,uint256 inputTokenAmount,uint256 minOutputTokenAmount,Transformation[] memory
// transformations)
static const uint32_t TRANSFORM_ERC20_SELECTOR = 0x415565b0;
// Selector: sailAdapterSwap(address,bytes,address,uint256,address,uint256,address)
// Full Signature: function sailAdapterSwap(address target,bytes memory callData,IERC20TokenV06
// tokenIn,uint256 tokenInAmount,IERC20TokenV06 tokenOut,uint256 minTokenOutAmount,address
// tokenOutRecipient)
static const uint32_t SAIL_ADAPTER_SWAP_SELECTOR = 0x1342555a;
// Selector:
// sailAdapterSwapWithFee(address,bytes,address,uint256,address,uint256,address,address,uint256)
// Full Signature: function sailAdapterSwapWithFee(address target,bytes memory
// callData,IERC20TokenV06 tokenIn,uint256 tokenInAmount,IERC20TokenV06 tokenOut,uint256
// minTokenOutAmount,address tokenOutRecipient,address payable feeRecipient,uint256 feeAmount)
static const uint32_t SAIL_ADAPTER_SWAP_WITH_FEE_SELECTOR = 0x9479c206;
// Selector: sellEthForTokenToUniswapV3(bytes,uint256,address)
// Full Signature: function sellEthForTokenToUniswapV3(bytes memory encodedPath,uint256
// minBuyAmount,address recipient)
static const uint32_t SELL_ETH_FOR_TOKEN_TO_UNISWAP_V3_SELECTOR = 0x3598d8ab;
// Selector: sellTokenForEthToUniswapV3(bytes,uint256,uint256,address)
// Full Signature: function sellTokenForEthToUniswapV3(bytes memory encodedPath,uint256
// sellAmount,uint256 minBuyAmount,address payable recipient)
static const uint32_t SELL_TOKEN_FOR_ETH_TO_UNISWAP_V3_SELECTOR = 0x803ba26d;
// Selector: sellTokenForTokenToUniswapV3(bytes,uint256,uint256,address)
// Full Signature: function sellTokenForTokenToUniswapV3(bytes memory encodedPath,uint256
// sellAmount,uint256 minBuyAmount,address recipient)
static const uint32_t SELL_TOKEN_FOR_TOKEN_TO_UNISWAP_V3_SELECTOR = 0x6af479b2;
// Selector: sellToPancakeSwap(address[],uint256,uint256,uint8)
// Full Signature: function sellToPancakeSwap(IERC20TokenV06[] calldata tokens,uint256
// sellAmount,uint256 minBuyAmount,ProtocolFork fork)
static const uint32_t SELL_TO_PANCAKESWAP_SELECTOR = 0xc43c9ef6;
// Selector: sailUniswapV3Swap(uint256,uint256,uint256[])
// Full Signature: function sailUniswapV3Swap(uint256 amount,uint256 minReturn,uint256[] memory
// pools)
static const uint32_t SAIL_UNISWAP_V3_SWAP_SELECTOR = 0x1b328275;
// Selector: sailUniswapV3SwapWithFee(uint256,uint256,uint256[],address,uint256,address)
// Full Signature: function sailUniswapV3SwapWithFee(uint256 swapAmount,uint256 minReturn,uint256[]
// memory pools,IERC20 sellToken,uint256 sellTokenFeeAmount,address payable feeRecipient)
static const uint32_t SAIL_UNISWAP_V3_SWAP_WITH_FEE_SELECTOR = 0xb8c329fc;
// Selector: sailUniswapV3SwapWithSlippage(uint256,uint256,uint256,uint256[],address,address)
// Full Signature:
static const uint32_t SAIL_UNISWAP_V3_SWAP_WITH_SLIPPAGE_SELECTOR = 0x4fe82bb9;
// Selector:
// sailUniswapV3SwapWithFeeAndSlippage(uint256,uint256,uint256,uint256[],address,address,uint256,address,address)
// Full Signature: function sailUniswapV3SwapWithFeeAndSlippage(uint256 swapAmount,uint256
// minReturn,uint256 expectedAmount,uint256[] memory pools,IERC20 sellToken,IERC20 buyToken,uint256
// sellTokenFeeAmount,address payable feeRecipient,address payable slippageRecipient)
static const uint32_t SAIL_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE_SELECTOR = 0x6100f308;
// Selector: sailSplitUniswapV3Swap(uint256[],uint256,uint256[][])
// Full Signature:  function sailSplitUniswapV3Swap(uint256[] calldata amount,uint256
// minReturn,uint256[][] calldata pools)
static const uint32_t SAIL_SPLIT_UNISWAP_V3_SWAP_SELECTOR = 0x318ced5d;
// Selector:
// sailSplitUniswapV3SwapWithFee(uint256[],uint256,uint256[][],address,address,uint256,address) Full
// Selector: function sailSplitUniswapV3SwapWithFee(uint256[] calldata amount,uint256
// minReturn,uint256[][] calldata pools,IERC20 buyToken,IERC20 sellToken,uint256
// sellTokenFeeAmount,address payable feeRecipient)
static const uint32_t SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE_SELECTOR = 0x985852c5;
// Selector:
// sailSplitUniswapV3SwapWithSlippage(uint256[],uint256,uint256,uint256[][],address,address,address)
// Full Signature: function sailSplitUniswapV3SwapWithSlippage(uint256[] calldata amount,uint256
// minReturn,uint256 expectedAmount,uint256[][] calldata pools,IERC20 buyToken,IERC20
// sellToken,address payable slippageRecipient)
static const uint32_t SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_SLIPPAGE_SELECTOR = 0x8253ac8b;
// Selector:
// sailSplitUniswapV3SwapWithFeeAndSlippage(uint256[],uint256,uint256,uint256[][],address,address,uint256,address,address)
// Full Signature: function sailSplitUniswapV3SwapWithFeeAndSlippage(uint256[] memory amount,uint256
// minReturn,uint256 expectedAmount,uint256[][] calldata pools,IERC20 sellToken,IERC20
// buyToken,uint256 sellTokenFeeAmount,address payable feeRecipient,address payable
// slippageRecipient)
static const uint32_t SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE_SELECTOR = 0x5bd4e83c;
// Selector: anyRecipientTransformERC20(address,address,address,uint256,uint256,(uint32,bytes)[])
// Full Signature: function anyRecipientTransformERC20(address payable recipient,IERC20TokenV06
// inputToken,IERC20TokenV06 outputToken,uint256 inputTokenAmount,uint256
// minOutputTokenAmount,ITransformERC20Feature.Transformation[] memory transformations)
static const uint32_t ANY_RECIPIENT_TRANSFORM_ERC20_SELECTOR = 0x351eb598;

// Array of Changelly selectors. Make sure this follows the same order as the
// enum defined in `chnagelly_plugin.h`
const uint32_t CHANGELLY_SELECTORS[NUM_SELECTORS] = {
    SELL_TO_UNISWAP_SELECTOR,
    SELL_TO_LIQUIDITY_PROVIDER_SELECTOR,
    TRANSFORM_ERC20_SELECTOR,
    SAIL_ADAPTER_SWAP_SELECTOR,
    SAIL_ADAPTER_SWAP_WITH_FEE_SELECTOR,
    SELL_ETH_FOR_TOKEN_TO_UNISWAP_V3_SELECTOR,
    SELL_TOKEN_FOR_ETH_TO_UNISWAP_V3_SELECTOR,
    SELL_TOKEN_FOR_TOKEN_TO_UNISWAP_V3_SELECTOR,
    SELL_TO_PANCAKESWAP_SELECTOR,
    SAIL_UNISWAP_V3_SWAP_SELECTOR,
    SAIL_UNISWAP_V3_SWAP_WITH_FEE_SELECTOR,
    SAIL_UNISWAP_V3_SWAP_WITH_SLIPPAGE_SELECTOR,
    SAIL_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE_SELECTOR,
    SAIL_SPLIT_UNISWAP_V3_SWAP_SELECTOR,
    SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE_SELECTOR,
    SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_SLIPPAGE_SELECTOR,
    SAIL_SPLIT_UNISWAP_V3_SWAP_WITH_FEE_AND_SLIPPAGE_SELECTOR,
    ANY_RECIPIENT_TRANSFORM_ERC20_SELECTOR};

// Used to indicate that the recipient should be the sender.
const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Function to dispatch calls from the ethereum app.
void dispatch_plugin_calls(int message, void *parameters) {
    switch (message) {
        case ETH_PLUGIN_INIT_CONTRACT:
            handle_init_contract(parameters);
            break;
        case ETH_PLUGIN_PROVIDE_PARAMETER:
            handle_provide_parameter(parameters);
            break;
        case ETH_PLUGIN_FINALIZE:
            handle_finalize(parameters);
            break;
        case ETH_PLUGIN_PROVIDE_INFO:
            handle_provide_token(parameters);
            break;
        case ETH_PLUGIN_QUERY_CONTRACT_ID:
            handle_query_contract_id(parameters);
            break;
        case ETH_PLUGIN_QUERY_CONTRACT_UI:
            handle_query_contract_ui(parameters);
            break;
        default:
            PRINTF("Unhandled message %d\n", message);
            break;
    }
}

void handle_query_ui_exception(unsigned int *args) {
    switch (args[0]) {
        case ETH_PLUGIN_QUERY_CONTRACT_UI:
            ((ethQueryContractUI_t *) args[1])->result = ETH_PLUGIN_RESULT_ERROR;
            break;
        default:
            break;
    }
}

// Calls the ethereum app.
void call_app_ethereum() {
    unsigned int libcall_params[5];

    libcall_params[0] = (unsigned int) "Ethereum";
    libcall_params[1] = 0x100;
    libcall_params[2] = RUN_APPLICATION;
    libcall_params[3] = (unsigned int) NULL;
#ifdef HAVE_NBGL
    caller_app_t capp;
    const char name[] = APPNAME;
    nbgl_icon_details_t icon_details;
    uint8_t bitmap[sizeof(ICONBITMAP)];

    memcpy(&icon_details, &ICONGLYPH, sizeof(ICONGLYPH));
    memcpy(&bitmap, &ICONBITMAP, sizeof(bitmap));
    icon_details.bitmap = (const uint8_t *) bitmap;
    capp.name = (const char *) name;
    capp.icon = &icon_details;
    libcall_params[4] = (unsigned int) &capp;
#else
    libcall_params[4] = (unsigned int) NULL;
#endif
    os_lib_call((unsigned int *) &libcall_params);
}

// Weird low-level black magic. No need to edit this.
__attribute__((section(".boot"))) int main(int arg0) {
    // Exit critical section
    __asm volatile("cpsie i");

    // Ensure exception will work as planned
    os_boot();

    // Try catch block. Please read the docs for more information on how to use
    // those!
    BEGIN_TRY {
        TRY {
            // Low-level black magic.
            check_api_level(CX_COMPAT_APILEVEL);

            // Check if we are called from the dashboard.
            if (!arg0) {
                // Called from dashboard, launch Ethereum app
                call_app_ethereum();
                return 0;
            } else {
                // Not called from dashboard: called from the ethereum app!
                const unsigned int *args = (const unsigned int *) arg0;

                // If `ETH_PLUGIN_CHECK_PRESENCE` is set, this means the caller is just
                // trying to know whether this app exists or not. We can skip
                // `dispatch_plugin_calls`.
                if (args[0] != ETH_PLUGIN_CHECK_PRESENCE) {
                    dispatch_plugin_calls(args[0], (void *) args[1]);
                }
            }
        }
        CATCH_OTHER(e) {
            switch (e) {
                // These exceptions are only generated on handle_query_contract_ui()
                case 0x6502:
                case EXCEPTION_OVERFLOW:
                    handle_query_ui_exception((unsigned int *) arg0);
                    break;
                default:
                    break;
            }
            PRINTF("Exception 0x%x caught\n", e);
        }
        FINALLY {
            // Call `os_lib_end`, go back to the ethereum app.
            os_lib_end();
        }
    }
    END_TRY;

    // Will not get reached.
    return 0;
}
