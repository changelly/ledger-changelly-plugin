import { ethers } from 'ethers';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x1aaad07998466cd3eb8140827dddb37570be1e63';
const chainID = 1; // Ethereum

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] sail_split_uniswap_v3_swap',
    zemu(model, async (sim, eth) => {
      //SailSplitUniswapV3Swap 0x318ced5d
      const inputData =
        '0x318ced5d0000000000000000000000000000000000000000000000000000000000000060000000000000000000000000000000000000000000000052736a14197a86bead00000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000054da07d800000000000000000000000000000000000000000000000000000000047743c80000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000028000000000000000000000001a349a3397a8431eed8d94a05f88f9001117fcaa00000000000000000000000075bff91af9878f5ec3fede9b52d51159afc2430a000000000000000000000000000000000000000000000000000000000000000380000000000000000000000011b815efb8f581194ae79006d24e0d814b7697f68000000000000000000000004585fe77225b41b697c938b018e2ac67ac5a20c0000000000000000000000000391e8501b626c623d39474afca6f9e46c268664989b198646787380000';

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
      const right_clicks = model.letter === 'S' ? 7 : 5;

      // Wait for the application to actually load and parse the transaction
      await waitForAppScreen(sim);
      // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
      await sim.navigateAndCompareSnapshots('.', model.name + 'sail_split_uniswap_v3_swap', [
        right_clicks,
        0,
      ]);

      await tx;
    })
  );
});
