#include "include/getmyblindaccounts.hpp"

#include <panels/commands.hpp>
#include <panels/cli.hpp>

GetMyBlindAccounts::GetMyBlindAccounts(GWallet* gwallet) : wxScrolledWindow() {
   p_GWallet = gwallet;

   InitWidgetsFromXRC((wxWindow *) p_GWallet);

   SetScrollRate(1, 1);

   response_grid->CreateGrid(0, 3);

   SetScrollRate(1, 1);
   response_grid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER);

   gwallet->panels.p_commands->DoGridProperties(response_grid);

   response_grid->SetColLabelValue(0, _("Label"));
   response_grid->SetColLabelValue(1, _("Public key"));
   response_grid->SetColLabelValue(2, _("Balances"));

   std::stringstream command;
   command << "get_my_blind_accounts";

   auto response =
         p_GWallet->panels.p_commands->ExecuteGetterCommand < map < string, public_key_type>>(command.str(),
               false, _("No blind accounts"));

   if (!response.IsNull()) {
      auto results = response.As<map<string, public_key_type>>();
      auto counter = 0;

      response_grid->BeginBatch();
      response_grid->ClearSelection();

      response_grid->AppendRows(results.size());
      for (auto &blind : results) {

         response_grid->SetCellValue(counter, 0, blind.first);
         response_grid->SetCellValue(counter, 1, string(blind.second));

         std::stringstream balances_string;

         const auto balances = p_GWallet->bitshares.wallet_api_ptr->get_blind_balances(blind.first);
         if(balances.size() > 0)
         {
            for(auto &balance : balances)
            {
               // Todo: create a function
               const std::string asset_id = fc::to_string(balance.asset_id.space_id) +
                                      "." + fc::to_string(balance.asset_id.type_id) +
                                      "." + fc::to_string(balance.asset_id.instance.value);

               const auto asset = p_GWallet->bitshares.wallet_api_ptr->get_asset(asset_id);
               balances_string << asset.symbol << ":" << p_GWallet->DoPrettyBalance(asset.precision, balance.amount.value) << " ";
            }
         }
         response_grid->SetCellValue(counter, 2, balances_string.str());

         ++counter;
      }
      response_grid->AutoSize();
      response_grid->ForceRefresh();
      response_grid->EndBatch();
   }
}
