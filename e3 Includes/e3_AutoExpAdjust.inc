
#event AutoExpAdjust "You gain#*#experience!#*#"

Sub Event_AutoExpAdjust
  /if (!${Defined[genSettings_Ini]}) /declare genSettings_Ini string outer ${${IniMode}[${MacroData_Ini},File Paths,General Settings]}
  /if (!${Defined[MaxLevel]}) /call #IniToVar "${genSettings_Ini},General,Max Level" MaxLevel int outer

  /if (${Me.Level}<${MaxLevel}) {
    /squelch /alternateadv on 0
    /return
  }
  /declare lowerExpThreshold int local 80
  /declare upperExpThreshold int local 90
  /declare currentExpToAA int local ${Window[AAWindow].Child[AAW_PercentCount].Text.Left[-1]}
  /declare currentExp float local ${Me.PctExp}

  /if (${currentExp}<=${lowerExpThreshold}) {
    /squelch /alternateadv on 0
  } else /if (${currentExp}>=${upperExpThreshold}) {
    /squelch /alternateadv on 100
  }
/return