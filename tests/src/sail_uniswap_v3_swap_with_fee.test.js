import { ethers } from 'ethers';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x1aaad07998466cd3eb8140827dddb37570be1e63';
const chainID = 1; // Ethereum

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] sail_uniswap_v3_swap_with_fee',
    zemu(model, async (sim, eth) => {
      // SailUniswapV3SwapWithFee 0xb8c329fc
      const inputData =
        '0xb8c329fc0000000000000000000000000000000000000000000000000000000059514ba000000000000000000000000000000000000000000000005277d8a95c4b66b24a00000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec7000000000000000000000000000000000000000000000000000000000016e360000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec700000000000000000000000000000000000000000000000000000000000000028000000000000000000000001a349a3397a8431eed8d94a05f88f9001117fcaa00000000000000000000000075bff91af9878f5ec3fede9b52d51159afc2430a64aabd64676EED0000';

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
      await sim.navigateAndCompareSnapshots('.', model.name + 'sail_uniswap_v3_swap_with_fee', [
        right_clicks,
        0,
      ]);

      await tx;
    })
  );
});
