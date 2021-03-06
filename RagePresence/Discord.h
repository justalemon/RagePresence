#pragma once

/// <summary>
/// Initializes the Discord RPC Library.
/// </summary>
void InitializeDiscord();
/// <summary>
/// If the Discord RPC Client is ready for communication.
/// </summary>
/// <returns>true if the Client is ready, false otherwise.</returns>
bool IsReady();
/// <summary>
/// Does cleanup of Discord RPC Information.
/// </summary>
void DoCleanup();
