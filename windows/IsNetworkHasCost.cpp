#include <winrt/Windows.Networking.Connectivity.h>
bool IsNetworkHasCost() {
	winrt::init_apartment();
	auto internetConnectionProfile{winrt::Windows::Networking::Connectivity::NetworkInformation::GetInternetConnectionProfile()};
	auto ConnectionCost = internetConnectionProfile.GetConnectionCost();
	if(ConnectionCost.NetworkCostType() != winrt::Windows::Networking::Connectivity::NetworkCostType::Unrestricted)
		return false;
	else
		return true;
}
