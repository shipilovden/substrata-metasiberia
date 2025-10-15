/*=====================================================================
AccountHandlers.h
-----------------
Copyright Glare Technologies Limited 2022 -
=====================================================================*/
#pragma once


#include <string>
class ServerAllWorldsState;
class User;


namespace web
{
class RequestInfo;
class ReplyInfo;
class UnsafeString;
}


/*=====================================================================
User account
-------------------

=====================================================================*/
namespace AccountHandlers
{
	void renderUserAccountPage(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	void renderUserAccountPageWin98(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderProveEthAddressOwnerPage(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderProveParcelOwnerByNFT(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void handleEthSignMessagePost(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderMakeParcelIntoNFTPage(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderParcelClaimSucceeded(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderParcelClaimFailed(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderParcelClaimInvalid(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderMakingParcelIntoNFT(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderMakingParcelIntoNFTFailed(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void handleMakeParcelIntoNFTPost(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void handleClaimParcelOwnerByNFTPost(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderScriptLog(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void renderSecretsPage(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void handleAddSecretPost(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);
	
	void handleDeleteSecretPost(ServerAllWorldsState& world_state, const web::RequestInfo& request_info, web::ReplyInfo& reply_info);

	void test();


	// Recover an Ethereum address from a signature and message
	std::string ecrecover(const std::string& sig, const std::string& msg); // hex-encoded sig, plain text msg
}
