|----------------------------------------------------------------------------|
|- e3_Classes_Shaman.inc v3.0 ( originally designed to work with e3.mac ) 	-|
|- Written by Killians of PEQ												                        -|
|----------------------------------------------------------------------------|
SUB check_Canni
/if (${Debug}) /echo |- autoCanni ==>
	/if (${AutoCanni}) {
	  |only canni if im not in ooc regen
		/if (${Me.CombatState.NotEqual[RESTING]}) {
			/declare s int local
	    |add special check for spiritual channeling
			/for s 1 to ${canniSpells2D.Size[1]}
			  /if (${canniSpells2D[${s},${iCastName}].NotEqual[Spiritual Channeling]} || (${canniSpells2D[${s},${iCastName}].Equal[Spiritual Channeling]} && ${Me.CombatState.Equal[COMBAT]})) {
          /call check_Ready "canniSpells2D" ${s}
          /if (${Macro.Return}) {
            /call check_Mana "canniSpells2D" ${s}
            /if (${Macro.Return}) {
            	/if (!${canniSpells2D[${s},${iCastName}].Equal[Cannibalization]} || ${Me.CurrentHPs} > 2000) {
								/if (${canniSpells2D[${s},${iMinHP}]} <= ${Me.PctHPs}) {
              /call e3_Cast ${Me.ID} "canniSpells2D" ${s}
            }
          }
        }
          }
        }
			/if (!${ActionTaken}) /next s
		}
	}
/if (${Debug}) /echo <== autoCanni -|
/RETURN

|----------------------------------------------------------------------------|
Sub SHM_Setup
/if (${Debug}) /echo |- SHM_Setup ==>
	/call IniToVar "${Character_Ini},Shaman,Auto-Canni (On/Off)" AutoCanni bool outer
	/call #IniToArray "${Character_Ini},Shaman,Canni#" canniSpells
  /if (${canniSpells.Size}) /call BuildSpellArray "canniSpells"	"canniSpells2D"
/if (${Debug}) /echo <== SHM_Setup -|
/return

|----------------------------------------------------------------------------|
SUB SHM_MacroSettings
/if (${Debug}) /echo |- SHM_MacroSettings ==>
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Debuffs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Burns" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Heals" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Cures" 1
  /call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Buffs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_DoTs" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_AE" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Nukes" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Pets" 1
	/call WriteToIni "${advSettings_Ini},SHM Functions,SHM Function" "check_Canni" 1
/if (${Debug}) /echo <== SHM_MacroSettings -|
/RETURN

|----------------------------------------------------------------------------|
SUB SHM_CharacterSettings
/if (${Debug}) /echo |- SHM_CharacterSettings ==>
	/call WriteToIni "${Character_Ini},Shaman,Auto-Canni (On/Off)"
	/call WriteToIni "${Character_Ini},Shaman,Canni"
/if (${Debug}) /echo <== SHM_CharacterSettings -|
/RETURN

|----------------------------------------------------------------------------|
Sub SHM_Aliases
/return

|----------------------------------------------------------------------------|
#event AE_AA "#*# tells you, 'AEAA'"
sub Event_AE_AA(string line)
	/if (${Me.Class.ShortName.Equal[SHM]}) {
   /if (${Me.AltAbilityReady[Mass Group Buff]} && ${Me.AltAbilityReady[Ancestral Aid]}) {
      /bc MGB Ancestral Aid inc...
     /casting "Mass Group Buff|alt" "-maxtries|3"
     /delay 5 !${Me.AltAbilityReady[Mass Group Buff]}
     /delay 5
     /casting "Ancestral Aid|alt" "-maxtries|3"
     /delay 5 !${Me.AltAbilityReady[Ancestral Aid]}
     /casting "Ancestral Aid|alt" "-maxtries|3"
      /rs MGB Ancestral Aid inc...
   } else /if (!${Me.AltAbilityReady[Mass Group Buff]}) {
      /bc Mass Group Buff is not available...
   } else /bc Ancestral Aid is not available...
	}
/return

|----------------------------------------------------------------------------|
Sub SHM_Background_Events
	/doevents AE_AA
/return