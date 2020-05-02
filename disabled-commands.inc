#define MAX_DISABLED_COMMANDS       (100)
#define COMMANDS_FILENAME           "commands.ini"

#include <zcmd>
#include <YSI_Storage\y_ini>

// Optional.
#tryinclude <crashdetect>
#tryinclude <YSI_Core\y_debug>

/**
 * FORWARDS
 */
forward INTL_OnReloadDisabledCommands();
forward OnDisabledCommandsReloaded();
forward OnPlayerCommandCancelled(playerid, cmdtext[]);

/**
 * GLOBAL VARIABLES
 */

new
  // Check is this feature disabled.
  bool:g_IsEnabled = false,
  // Check for looping.
  g_CurrentIndex = -1,
  // For storing maximum disabled commands.
  g_DisabledCommands[MAX_DISABLED_COMMANDS][MAX_FUNC_NAME];

/**
 * FILE HANDLER
 */
INI:commands[list](name[], value[])
{
  // Convert the value to boolean.
  new
    bool:IsDisabled = strcmp(value, "true", true) == 0 ? true : false;

  // Only store the disabled commands if the amount is below MAX_DISABLED_COMMANDS
  if (g_CurrentIndex < MAX_DISABLED_COMMANDS)
  {
    if (IsDisabled == true)
    {
      g_CurrentIndex++;
      format(g_DisabledCommands[g_CurrentIndex], MAX_FUNC_NAME, "/%s", name);
    }

    // Print detected commands in the file.
    printf("[INFO] Command: /%s | Is disabled? %s | Amount of disabled commands: %d", name, IsDisabled ? "TRUE" : "FALSE", g_CurrentIndex + 1);
  }
  else
  {
    // Print the output when maximum disabled commands exceeded.
    printf("[WARN] Maximum disabled commands reached. Maximum: %d | Unprocessed command: %s", MAX_DISABLED_COMMANDS, name);
  }

  return 0;
}

InitDisabledCommands()
{
  // Open the file.
  new
    INI:iniFile = INI_Open(COMMANDS_FILENAME);

  if (iniFile == INI_NO_FILE)
  {
    print("[ERROR] File \""#COMMANDS_FILENAME"\" not created in \"scriptfiles\". Please create it manually.");
  }
  else
  {
    // Suppress the warning.
    new filename[MAX_FUNC_NAME] = "";
    format(filename, sizeof(filename), "%s", COMMANDS_FILENAME);

    INI_SetTag(iniFile, "list");
    INI_Load(filename);

    // Enable the feature.
    g_IsEnabled = true;
  }

  // Close the file.
  g_CurrentIndex = 1;
  INI_Close(iniFile);
}

ResetDisabledCommands()
{
  // Reset all disabled commands.
  for (new i = 0; i < MAX_DISABLED_COMMANDS; i++)
  {
    format(g_DisabledCommands[i], MAX_FUNC_NAME, "");
  }
}

_ReloadDisabledCommands()
{
  // Disable the feature first.
  g_IsEnabled = false;

  // Then, reset the disabled commands and reload from the file.
  ResetDisabledCommands();
  InitDisabledCommands();
  return 1;
}

public OnPlayerCommandReceived(playerid, cmdtext[])
{
  // Always check the feature.
  if (g_IsEnabled == true)
  {
    for (new i = 0; i < MAX_DISABLED_COMMANDS; i++)
    {
      if (isnull(g_DisabledCommands[i]) == true)
      {
        #if defined _INC_y_core
        P:3("[DEBUG] COMMAND SKIPPED -> index: %d | cmdtext: %s | g_DisabledCommands[i]: %s", i, cmdtext, g_DisabledCommands[i]);
        // P:3("[DEBUG] COMMAND SKIPPED -> strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME) == %d", strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME));
        #endif
        continue;
      }

      // Ignore empty command.
      if (isnull(cmdtext) == true)
      {
        return 0;
      }

      // Debugging with y_debug if _DEBUG defined and y_debug is included.
      #if defined _INC_y_core
      P:3("[DEBUG] index: %d | cmdtext: %s | g_DisabledCommands[i]: %s", i, cmdtext, g_DisabledCommands[i]);
      // P:3("[DEBUG] strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME) == %d", strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME));
      #endif

      // Compare the command.
      if (strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME) == 0)
      {
        #if defined _INC_y_core
        P:3("[DEBUG] COMMAND MATCHED -> index: %d | cmdtext: %s | g_DisabledCommands[i]: %s | strcmp(...): %d", i, cmdtext, g_DisabledCommands[i], strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME));
        // P:3("[DEBUG] strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME) == %d", strcmp(cmdtext, g_DisabledCommands[i], true, MAX_FUNC_NAME));
        #endif

        // Backtracing if needed.
        #if defined CRASHDETECT_INC
        PrintBacktrace();
        #endif

        // Call the callback that command is cancelled.
        CallLocalFunction("OnPlayerCommandCancelled", "is", playerid, cmdtext);
        return 0;
      }
    }
  }

  #if defined _INC_y_core
  P:3("[DEBUG] OnPlayerCommandReceived PASEED -> playerid: %d | cmdtext: %s", playerid, cmdtext);
  #endif
  return 1;
}

#if defined FILTERSCRIPT
// It's a FilterScript.
public OnFilterScriptInit()
{
  InitDisabledCommands();
  return CallLocalFunction("dcom_OnFilterScriptInit", "");
}

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit dcom_OnFilterScriptInit
forward dcom_OnFilterScriptInit();

#else
// It's a GameMode.
public OnGameModeInit()
{
  InitDisabledCommands();
  return CallLocalFunction("dcom_OnGameModeInit", "");
}

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit dcom_OnGameModeInit
forward dcom_OnGameModeInit();

#endif

/**
 * CALLBACKS
 */
public INTL_OnReloadDisabledCommands()
{
  _ReloadDisabledCommands();
  CallLocalFunction("OnDisabledCommandsReloaded", "");
  return 1;
}

/**
 * STOCKS
 */
stock ReloadDisabledCommands()
{
  CallRemoteFunction("INTL_OnReloadDisabledCommands", "");
  return 1;
}

stock AddDisabledCommands(const command[])
{
  #if defined _INC_y_core
  P:3("[DEBUG] stock AddDisabledCommands CALLED -> command: %s", command);
  #endif

  if (isnull(command) == true)
  {
    #if defined _INC_y_core
    P:3("[DEBUG] stock AddDisabledCommands NULL COMMAND -> command: %s", command);
    #endif
    return 0;
  }

  if (command[0] == '/')
  {
    #if defined _INC_y_core
    P:3("[DEBUG] stock AddDisabledCommands HAS SLASH -> command: %s", command);
    #endif
    return 0;
  }

  #if defined _INC_y_core
  P:3("[DEBUG] stock AddDisabledCommands PASSED -> command: %s", command);
  #endif

  new INI:iniFile = INI_Open(COMMANDS_FILENAME);

  INI_SetTag(iniFile, "list");
  INI_WriteBool(iniFile, command, true);
  INI_Close(iniFile);

  ReloadDisabledCommands();
  return 1;
}

stock RemoveDisabledCommands(const command[])
{
  #if defined _INC_y_core
  P:3("[DEBUG] stock RemoveDisabledCommands CALLED -> command: %s", command);
  #endif

  if (isnull(command) == true)
  {
    #if defined _INC_y_core
    P:3("[DEBUG] stock RemoveDisabledCommands NULL COMMAND -> command: %s", command);
    #endif
    return 0;
  }

  if (command[0] == '/')
  {
    #if defined _INC_y_core
    P:3("[DEBUG] stock RemoveDisabledCommands HAS SLASH -> command: %s", command);
    #endif
    return 0;
  }

  #if defined _INC_y_core
  P:3("[DEBUG] stock RemoveDisabledCommands PASSED -> command: %s", command);
  #endif

  new INI:iniFile = INI_Open(COMMANDS_FILENAME);

  INI_SetTag(iniFile, "list");
  INI_RemoveEntry(iniFile, command);
  INI_Close(iniFile);

  ReloadDisabledCommands();
  return 1;
}