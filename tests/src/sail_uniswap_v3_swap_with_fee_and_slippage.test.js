import { ethers } from 'ethers';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x1aaad07998466cd3eb8140827dddb37570be1e63';
const chainID = 1; // Ethereum

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] sail_uniswap_v3_swap_with_fee_and_slippage',
    zemu(model, async (sim, eth) => {
      // SailUniswapV3SwapWithFeeAndSlippage 0x6100f308
      const inputData =
        '0x6100f3080000000000000000000000000000000000000000000000000000000059514ba000000000000000000000000000000000000000000000005277d8a95c4b66b24a0000000000000000000000000000000000000000000000528cfab44af624af640000000000000000000000000000000000000000000000000000000000000120000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec70000000000000000000000006b175474e89094c44da98b954eedeac495271d0f000000000000000000000000000000000000000000000000000000000016e360000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec70000000000000000000000002e05327c61784aac486cb9cedc71594c0f54644100000000000000000000000000000000000000000000000000000000000000028000000000000000000000001a349a3397a8431eed8d94a05f88f9001117fcaa00000000000000000000000075bff91af9878f5ec3fede9b52d51159afc2430ad2102b646770360000';

      // Get the generic transaction template
      let unsignedTx = genericTx;
      // Adapt to the appropriate network
      unsignedTx.chainId = chainID;
      // Modify `to` to make it interact with the contract
      unsignedTx.to = contractAddr;
      // Modify the attached data
      unsignedTx.data = inputData;

      // Create serializedTx and remove the "0x" prefix
      const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
      const right_clicks = model.letter === 'S' ? 8 : 7;

      // Wait for the application to actually load and parse the transaction
      await waitForAppScreen(sim);
      // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
      await sim.navigateAndCompareSnapshots('.', model.name + 'sail_uniswap_v3_swap_with_fee_and_slippage', [
        right_clicks,
        0,
      ]);

      await tx;
    })
  );
});
