|
| FindAugsCmd.inc r1
|   Adds eqbc chat commands for listing unused augs.
|   Run these commands to create aliases:
|     /alias /whitelistaug /echo WhiteList Aug
|     /alias /blacklistaug /echo BlackList Aug
|     /alias /findaugs /bc FindAugs
|     /alias /findotheraugs /bc FindOtherAugs
|
|   Augs will only be reported if they are on the white list.
|   To add an aug to the whitelist: (Item links may be used)
|     /whitelistaug Super Cool Augmentation of Doom
|   
|   Use /blacklistaug to place an aug on blacklist. A blacklisted aug will never be displayed.
|
|   Use /findotheraugs to find augs currently not placed on either list. It will not display blacklisted augs.
|

#Event WhiteListAug "[MQ2] WhiteList Aug #1#"
#Event BlackListAug "[MQ2] BlackList Aug #1#"

#Event FindAugsCmd "<#1#> FindAugs#2#"
Sub Event_FindAugsCmd(line, ChatSender, eventParams)

	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/if (${Debug}) /bc ChatSender: ${ChatSender} User: ${Bool[${Macro.Return.Arg[2,_]}]} Zone: ${Bool[${Macro.Return.Arg[4,_]}]} In Range: ${Bool[${Macro.Return.Arg[5,_]}]}
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
  	/if (!${Defined[facReturn]}) /declare facReturn string outer
		/call ForEachAug DisplayIfWhiteListed
	} else {
	}
/return

#Event FindOtherAugsCmd "<#1#> FindOtherAugs#2#"
Sub Event_FindOtherAugsCmd(line, ChatSender, eventParams)
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
		/if (!${Defined[facReturn]}) /declare facReturn string outer
		/call ForEachAug DisplayIfUnlisted
	} else {
	}
/return

Sub ForEachAug(callback)
  /declare packSlot int local
  /declare index int local
  /declare itemName string local
  /for index 23 to 30
    /if (${Me.Inventory[${index}].Container}) {
      /for packSlot 1 to 10
        /if (${Me.Inventory[${index}].Item[${packSlot}].ID} && ${Me.Inventory[${index}].Item[${packSlot}].AugType}) {
          /varset itemName ${Me.Inventory[${index}].Item[${packSlot}].Name}
          /call ${callback} "${itemName}"
        }
      /next packSlot
    } else {
      /if (${Me.Inventory[${index}].ID} && ${Me.Inventory[${index}].AugType}) {
        /varset itemName ${Me.Inventory[${index}].Name}
        /call ${callback} "${itemName}"
      }
    }
  /next index
/return

Sub DisplayIfWhiteListed(itemName)
  /if (!${Defined[facReturn]}) /declare facReturn string outer
  /call FAC_IsWhiteListed "${itemName}" facReturn
  /if (${facReturn}) /bc Found Aug: ${itemName}
/return

Sub DisplayIfUnlisted(itemName)
  /if (!${Defined[facReturn]}) /declare facReturn string outer
  /call FAC_IsWhiteListed "${itemName}" facReturn
  /if (!${facReturn}) {
    /call FAC_IsBlackListed "${itemName}" facReturn
    /if (!${facReturn}) {
      /bc Unlisted Aug: ${itemName}
    }
  }
/return

Sub FAC_IsWhiteListed(itemName, destVar)
  /varset ${destVar} ${Ini[${augfinder_ini},Whitelist,${itemName.Replace[ ,_].Replace[',].Replace[`,]},${Ini[augfinder.ini,Whitelist,All,0]}]}
/return

Sub FAC_IsBlackListed(itemName, destVar)
  /varset ${destVar} ${Ini[${augfinder_ini},Blacklist,${itemName.Replace[ ,_].Replace[',].Replace[`,]},0]}
/return

Sub Event_WhiteListAug(line, args)
  /if (${args.Find[]}) {
    /varset args ${args.Right[-51].Left[-1]}
  }
  /ini "${augfinder_ini}" "Whitelist" "${args.Replace[ ,_].Replace[',].Replace[`,]}" "1"
  /if (!${Defined[facReturn]}) /declare facReturn string outer
  /call FAC_IsBlackListed "${args}" facReturn
  /if (${facReturn}) {
    /ini "${augfinder_ini}" "Blacklist" "${args.Replace[ ,_].Replace[',].Replace[`,]}" "0"
  }
  /echo Added ${args} to Whitelist
/return

Sub Event_BlackListAug(line, args)
  /if (${args.Find[]}) {
    /varset args ${args.Right[-51].Left[-1]}
  }
  /ini "${augfinder_ini}" "Blacklist" "${args.Replace[ ,_].Replace[',].Replace[`,]}" "1"
  /if (!${Defined[facReturn]}) /declare facReturn string outer
  /if (!${Defined[facReturn]}) /declare facReturn string outer
  /call FAC_IsWhiteListed "${args}" facReturn
  /if (${facReturn}) {
    /ini "${augfinder_ini}" "Whitelist" "${args.Replace[ ,_].Replace[',].Replace[`,]}" "0"
  }
  /echo Added ${args} to Blacklist
/return

Sub FAC_GetAugSlots(itemName, destVar)
  /varset ${destVar}
  /declare Pow2Iter string local 1|2|4|8|16|32|64|128|256|512|1024|2048
  /declare i int local 0
  /declare itemAugType int local ${FindItem[${itemName}].AugType}
  /declare thisIter int local
  /for i 1 to ${Math.Calc[${Pow2Iter.Count[|]}+1]}
    /varset thisIter ${Pow2Iter.Arg[${i},|]}
    /if ((${itemAugType} & ${thisIter}) == ${thisIter}) {
      /varset ${destVar} ${${destVar}}|${i}
    }
  /next i
  /varset ${destVar} ${${destVar}}|
/return

Sub FAC_OldFilterByAugSlots
  |old code from main function, may want functionality later, idk
  /declare argIndex int local
  /declare showAug int local
  /if (!${Defined[AugSlotsFound]}) /declare AugSlotsFound string outer
  /call FAC_GetAugSlots "${Me.Inventory[${index}].Item[${packSlot}].Name}" AugSlotsFound
  /if (${Defined[args]}) {
    /varset showAug 0
    /for argIndex 1 to ${Math.Calc[${args.Count[ ]}+1]}
      /if (${AugSlotsFound.Find[|${args.Arg[${argIndex}, ]}|]}) {
        /varset showAug 1
      }
    /next argIndex
    /if (${showAug}) {
      /bc Aug Found: ${Me.Inventory[${index}].Item[${packSlot}].Name} Slots: ${AugSlotsFound.Replace[|, ]}
    }
  } else {
    /bc Aug Found: ${Me.Inventory[${index}].Item[${packSlot}].Name} Slots: ${AugSlotsFound.Replace[|, ]}
  }
/return

Sub DisplayIfItemAugWhiteListed(augname, itemName)
  /call FAC_IsWhiteListed "${augname}" facReturn
  /if (${facReturn}) /bc Found Aug: ${itemName}: ${augname}
/return

Sub ForEachItem(callback)
  /declare packSlot int local
  /declare index int local
  /declare itemName string local
  /declare augName string local
  /declare augcount int local
  /declare augs int local

  /for index 23 to 30
    /if (${Me.Inventory[${index}].Container}) {
      /for packSlot 1 to 10
        /if (${Me.Inventory[${index}].Item[${packSlot}].ID} && ${Me.Inventory[${index}].Item[${packSlot}].Augs} > 0) {
          /varset augcount ${Me.Inventory[${index}].Augs}
          /for augs 1 to ${augcount}
            /varset augName ${Me.Inventory[${index}].Item[${packSlot}].Item[${augs}]}
            /varset itemName ${Me.Inventory[${index}].Item[${packSlot}].Name}
            /call ${callback} "${augName}" "${itemName}"
          /next augs
        }
      /next packSlot
    } else {
      /if (${Me.Inventory[${index}].ID} && ${Me.Inventory[${index}].Augs} > 0) {
        /varset augcount ${Me.Inventory[${index}].Augs}
        /for augs 1 to ${augcount}
          /varset augName ${Me.Inventory[${index}].Item[${augs}]}
          /varset itemName ${Me.Inventory[${index}].Name}
          /call ${callback} "${augName}" "${itemName}"
        /next augs
      }
    }
  /next index
/return

#Event FindAugedCmd "<#1#> FindAuged#2#"
Sub Event_FindAugedCmd(line, ChatSender, eventParams)
	| -Verify the event
	/call verifyEvent "${ChatSender}" "${line}"
	/varset ChatSender ${Macro.Return.Arg[1,_]}
	|    |-----------Valid User----------|    |--------Included Bot-----------|    |-----------In Zone-------------|    |------------In Range------------|
	/if (${Bool[${Macro.Return.Arg[2,_]}]} && ${Bool[${Macro.Return.Arg[3,_]}]} && ${Bool[${Macro.Return.Arg[4,_]}]} && ${Bool[${Macro.Return.Arg[5,_]}]}) {
  		/if (!${Defined[facReturn]}) /declare facReturn string outer
		/call ForEachItem DisplayIfItemAugWhiteListed
	} else {
	}
/return

Sub FindAugsCMD_Background_Events
  /doevents FindAugedCmd
  /doevents FindAugsCmd
  /doevents FindOtherAugsCmd
  /doevents WhiteListAug
  /doevents BlackListAug
/return

Sub FindAugsCMD_Aliases
  /alias /whitelistaug /echo WhiteList Aug
  /alias /blacklistaug /echo BlackList Aug
  /alias /findaugs /bc FindAugs
  /alias /findauged /bc FindAuged
  /alias /findotheraugs /bc FindOtherAugs
/return

Sub FindAugsCMD_Setup
  /declare augfinder_ini string outer e3 Macro Inis\findaugscmd.ini
/return

Sub FindAugsCMD_CharacterSettings
/return

Sub FindAugsCMD_MacroSettings
/return