import { ethers } from 'ethers';
import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, genericTx } from './test.fixture';

const contractAddr = '0x6018b292fddeaa83bb5d7b85415270b4fc6d0c12';
const chainID = 56; // BSC

nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] sell_to_pancake_swap_bsc',
    zemu(
      model,
      async (sim, eth) => {
        // sellToPancakeSwap 0xc43c9ef6
        const inputData =
          '0xc43c9ef60000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000041b1b1a3b3f4000000000000000000000000000000000000000000000000000040da54b046e06300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200000000000000000000000055d398326f99059ff775485246999027b31979550000000000000000000000001af3f329e8be154074d8769d1ffa4ee058b1dbc3d7472c6374c56b';

        // Get the generic transaction template
        let unsignedTx = genericTx;
        //adapt to the appropriate network
        unsignedTx.chainId = chainID;
        // Modify `to` to make it interact with the contract
        unsignedTx.to = contractAddr;
        // Modify the attached data
        unsignedTx.data = inputData;
        // unsignedTx.value = parseEther("0.41");

        // Create serializedTx and remove the "0x" prefix
        const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
        const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);
        // const right_clicks = 6;
        const right_clicks = model.letter === 'S' ? 8 : 6;

        // Wait for the application to actually load and parse the transaction
        await waitForAppScreen(sim);
        // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
        await sim.navigateAndCompareSnapshots('.', model.name + 'sell_to_pancake_swap_bsc', [
          right_clicks,
          0,
        ]);

        await tx;
      },
      'bsc'
    )
  );
});
