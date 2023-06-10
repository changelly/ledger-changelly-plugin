import 'regenerator-runtime/runtime';
import { waitForAppScreen, zemu, nano_models, txFromEtherscan } from './test.fixture';

// Test from replayed transaction: https://etherscan.io/tx/0x0160b3aec12fd08e6be0040616c7c38248efb4413168a3372fc4d2db0e5961bb
nano_models.forEach(function (model) {
  test(
    '[Nano ' + model.letter + '] Sell to uniswap',
    zemu(model, async (sim, eth) => {
      // The rawTx of the tx up above is accessible through: https://etherscan.io/getRawTx?tx=0xa1f19c8668303cbdc067e5b73b5400873a981230baadbdd54eca21ed4747342c
      const serializedTx = txFromEtherscan(
        '0x02f901970133850861c46800850861c468008302b9f2941aaad07998466cd3eb8140827dddb37570be1e6380b90128d9627aa400000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000001c9c38000000000000000000000000000000000000000000000000000503a1c86e1e17100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000a0b86991c6218b36c1d19d4a2e9eb0ce3606eb48000000000000000000000000eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee869584cd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000939c5ee9e3634eb91ec001a0882e19fe61589c1b40ffc3a91c248f0e88e7834dcc6a96641a43ac80faeed5b9a0223fa96046818ff945b708deb216271cf9f48fee440a05befbf78d93dd4bf275'
      );

      const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

      const right_clicks = model.letter === 'S' ? 7 : 5;

      // Wait for the application to actually load and parse the transaction
      await waitForAppScreen(sim);
      // Navigate the display by pressing the right button `right_clicks` times, then pressing both buttons to accept the transaction.
      await sim.navigateAndCompareSnapshots('.', model.name + 'sell-to-uniswap', [right_clicks, 0]);

      await tx;
    })
  );
});
