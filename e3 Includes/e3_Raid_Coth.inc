sub check_raid_coth
  /call Raid_Coth_Background_Events
/return

Sub Raid_Coth_Background_Events
  /doevents RCotH:CotHRaidTo
  /doevents RCotH:CoHInfo
/return

Sub Raid_Coth_Aliases
/return

Sub Raid_Coth_Setup
/return

Sub Raid_Coth_CharacterSettings
/return

Sub RCotH:Initialize
  /call #CreateVars string outer RCotH:SelectedPlayer RCotH:SelectedLocation RCotH:Return RCotH:RaidWindowUngroupedIndices RCotH:RaidWindowUngroupedSize RCotH:MageList
  /call #CreateVars int outer RCotH:RaidManifestStart RCotH:IAMCoHMaster RCotH:MageCount RCotH:CotHInProgress RCotH:RemainingCotH RCotH:StartTime
  /call #CreateArrays string outer RCotH:RaidManifest[72] RCotH:MageManifest[12]
  
  /varset RCotH:RaidManifestStart 1

  |/call #RequirePlugin MQ2TimeUtil
  |/if (${Macro.Return}) {
  |  /varset RCotH:StartTime ${Now}
  |}
  
  /declare i int local
  /for i 1 to 12
    /call #CreateVars string outer RCotH:RaidWindowGroupIndices${i}
    /varset RCotH:RaidWindowGroupIndices${i} ${Math.Calc[${Math.Calc[((${i}*6-5)/6)].Int}+${i}*6-6+1]} to ${Math.Calc[${Math.Calc[((${i}*6-5)/6)].Int}+${i}*6]}
  /next i
  
  /noparse /varset RCotH:RaidWindowUngroupedIndices 1 to ${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].Items}
  /noparse /varset RCotH:RaidWindowUngroupedSize ${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].Items}

  /declare RCotH:MageListTimer timer outer 50
/return

#Event RCotH:CotHRaidTo "#1# tell#*#raid,#*#'co#*#h#*# raid to #2#'"

Sub Event_RCotH:CotHRaidTo(line, chatSender, destination)
  /call #CreateVars string outer RCotH:PreviousLeader RCotH:Return
  /varset RCotH:PreviousLeader ${Raid.Leader.Name}
  /delay 15
  /if (${Defined[RCotH:IAMCoHMaster]}) /deletevar RCotH:IAMCoHMaster
  /if (!${Raid.Members}) /return
  /call #StripNameLink "${chatSender}" RCotH:Return
  /if (${RCotH:Return.Equal[You]}) /varset RCotH:Return ${Me.Name}
  /if (!${SpawnCount[pc =${RCotH:Return}]}) /return
  /if (${Select[${Me.Class.ShortName},MAG]}) {
    /call RCotH:OutputMageInfo
  }
  /if (${Raid.Leader.Name.Equal[${Me.Name}]}) {
    /if (${Me.Name.Equal[${destination}]}) {
      /call RCotH:StartRaidCotH
    } else {
      /rsay RCotH> I am raid leader.
      /call RCotH:MakeLeader ${destination}
    }
  } else /if (${Me.Name.Equal[${destination}]}) {
    /call RCotH:WaitForLeadership
    /if (${Raid.Leader.Name.Equal[${Me.Name}]}) {
      /call RCotH:StartRaidCotH
    }
  }
/return

Sub RCotH:StartRaidCotH
  /call RCotH:OpenWindow
  /call RCotH:Initialize
  /call RCotH:SnapshotRaidGroups
  /call RCotH:WriteManifest
  /call RCotH:CheckDistances
  /call RCotH:WaitForMageList
  /if (!${RCotH:MageList.Length}) {
    /rsay RCotH> No mages available for CotH! Ending Raid CotH.
    /call RCotH:Unlock
    /if (!${RCotH:PreviousLeader.Equal[${Me.Name}]}) /call RCotH:MakeLeader "${RCotH:PreviousLeader}"
    /call RCotH:CloseWindow
    /return
  }
  /call RCotH:Lock
  /call RCotH:UngroupAll
  /call RCotH:CotHMages
  /doevents flush RCotH:CoHInfo
  /call RCotH:ExecuteRaidCotH
  /call RCotH:UngroupAll
  /call RCotH:RestoreRaidGroups
  /call RCotH:Unlock
  /if (!${RCotH:PreviousLeader.Equal[${Me.Name}]}) /call RCotH:MakeLeader "${RCotH:PreviousLeader}"
  /call RCotH:CloseWindow
  /if (${RCotH:StartTime} > 0) {
    /declare timeTaken int outer ${Math.Calc[${Now} - ${RCotH:StartTime}]}
    /rsay Raid CotH is finished! Time taken: ${DurationS[${timeTaken}]}
  } else {
    /rsay Raid CotH is finished!
  }
/return

Sub RCotH:CotHMages
  /declare MageCount int local ${RCotH:MageList.Count[|]}
  /declare MagesNeedingCotH string local
  /declare i int local
  /declare a int local
  /declare b int local
  /declare iter int local
  /declare current string local
  
  /call RCotH:RemoveFromManifest ${Me.Name}
  
  /varcalc RCotH:MageCount 0
  
  /for i 1 to ${MageCount}
    /varset current ${RCotH:MageList.Arg[${i},|]}
    /varset RCotH:MageManifest[${i}] ${current}
    /varcalc RCotH:MageCount ${RCotH:MageCount}+1
    /if (${current.Equal[${Me.Name}]}) /next i
    /if (${Spawn[pc =${current}].Distance}>150) {
      /varset MagesNeedingCotH ${MagesNeedingCotH}${current}|
      /call RCotH:RemoveFromManifest ${current}
    }
    /if (${MagesNeedingCotH.Count[|]}==12) /goto :ThatsEnoughMages
  /next i
  :ThatsEnoughMages
  /if (!${MagesNeedingCotH.Length}) /return
  /varcalc a 1
  /varcalc MageCount ${MagesNeedingCotH.Count[|]}
  :MoreMages
    /call RCotH:SelectPlayer ${Me.Name} RAID_NotInGroupPlayerList "${RCotH:RaidWindowUngroupedIndices}"
    /call RCotH:MoveSelected 1
    /varcalc b ${MageCount}
    /if (${Math.Calc[(${b}-${a})]}>4) {
      /varcalc b ${a}+4
    }
    /for i ${a} to ${b}
      /call RCotH:SelectPlayer ${MagesNeedingCotH.Arg[${i},|]} RAID_NotInGroupPlayerList "${RCotH:RaidWindowUngroupedIndices}"
      /call RCotH:MoveSelected 1
    /next i
    /call RCotH:CotHMyGroup
    /call RCotH:Ungroup 1
    /varcalc a ${b}+1
    /if (${b} < ${MageCount}) /goto :MoreMages
  |/call RCotH:MovePlayer
/return

Sub RCotH:CotHMyGroup
  /declare i int local
  /declare counter int local
  :Recheck
  /if (${counter}==${Group.Members}) /goto :Finished
  /for i 1 to ${Group.Members}
    /call RCotH:NeedsCoH ${Group.Member[${i}].Name}
    /if (!${RCotH:Return}) /varcalc counter ${counter}+1
    /if (!${RCotH:Return}) /next i
    /if (!${Bool[${Group.Member[${i}].Name.Length}]}) /goto :Finished
    /gsay Summoning ${Group.Member[${i}]}
    :KeepWaiting
    /delay 25s ((${Me.SpellReady[Call of the Hero]} && ${Me.CurrentMana} > 600) || ${Me.AltAbilityReady[Call of the Hero]})
    /stand
    /if (${Me.AltAbilityReady[Call of the Hero]}) {
      /casting "Call of the Hero" alt -targetid|${Group.Member[${i}].ID} -maxtries|5
    } else /if (${Me.SpellReady[Call of the Hero]}) {
      /casting "Call of the Hero" gem|5 -targetid|${Group.Member[${i}].ID} -maxtries|5
    } else {
      /sit
      /goto :KeepWaiting
    }
    /delay 2s
    /delay 20s ${Cast.Status.Find[C]}
    /delay 20s !${Me.Casting.ID}
    /sit
    /delay 2s
    /call RCotH:NeedsCoH ${Group.Member[${i}].Name}
    /if (!${RCotH:Return}) /varcalc counter ${counter}+1
    /if (${Defined[RCotH:IAMCoHMaster]} && ${i}<5) /doevents RCotH:CoHInfo
  /next i
  /if (${counter}<${Group.Members}) /goto :Recheck
  :Finished
  /rsay RCotH> My group is done
/return

Sub RCotH:ExecuteRaidCotH
  /call RCotH:GetManifestCount
  /varset RCotH:RemainingCotH ${RCotH:Return}
  /declare i int local
  /for i 1 to ${RCotH:MageCount}
    /call #CreateVars int outer RCotH:MageGroup${RCotH:MageManifest[${i}]}
  /next i
  /for i 1 to ${RCotH:MageCount}
    /varset RCotH:MageGroup${RCotH:MageManifest[${i}]} ${i}
    /call RCotH:SelectPlayer ${RCotH:MageManifest[${i}]} RAID_NotInGroupPlayerList "${RCotH:RaidWindowUngroupedIndices}"
    /call RCotH:MoveSelected ${i}
  /next i
  /varset RCotH:CotHInProgress ${RCotH:MageCount}
  /for i 1 to ${RCotH:MageCount}
    /if (${RCotH:MageManifest[${i}].Equal[${Me.Name}]}) /next i
    /if (!${RCotH:RemainingCotH}) /return
    /call RCotH:MageAvailable "${RCotH:MageManifest[${i}]}"
  /next i
  /if (!${RCotH:RemainingCotH}) /return
  /call RCotH:MageAvailable ${Me.Name}
  
  :Loop
    /doevents RCotH:CoHInfo
    /delay 1
  /if (${RCotH:RemainingCotH}) /goto :Loop
  /if (${RCotH:CotHInProgress}) /goto :Loop
/return

Sub RCotH:MageAvailable(chatSender)
  /if (!${Defined[RCotH:MageGroup${chatSender}]}) /return
  /varcalc RCotH:CotHInProgress ${RCotH:CotHInProgress} - 1
  /if (!${RCotH:RemainingCotH}) /return
  /varcalc RCotH:CotHInProgress ${RCotH:CotHInProgress} + 1
  /declare i int local
  /declare groupNum int local ${RCotH:MageGroup${chatSender}}  
  /call RCotH:UngroupLeaveLeader ${groupNum}
  /for i 1 to 5
    /if (!${RCotH:RemainingCotH}) /goto :BreakLoop
    /varcalc RCotH:RemainingCotH ${RCotH:RemainingCotH}-1
    /call RCotH:GetNextFromManifest
    /call RCotH:SelectPlayer "${RCotH:Return}" RAID_NotInGroupPlayerList "${RCotH:RaidWindowUngroupedIndices}"
    /call RCotH:MoveSelected ${groupNum}
  /next i
  :BreakLoop
  /if (${chatSender.Equal[${Me.Name}]}) {
    /call RCotH:CotHMyGroup
  } else {
    /rsay RCotH> Summon your group, ${chatSender}
  }
/return

Sub RCotH:WaitForMageList
  :Loop
    /doevents RCotH:CoHInfo
    /delay 1
    /if (${RCotH:MageListTimer}) /goto :Loop
    /deletevar RCotH:MageListTimer
/return

Sub RCotH:OutputMageInfo
  /if (${Me.Book[Call of the Hero]}) {
    /if (${FindItemCount[=Pearl]}<72) {
      /rsay RcotH> I need more pearls to join the CoH!
    } else {
      /rsay RCotH> I am available to CoH
      /if (${FindItemCount[=Pearl]}<=144) {
        /rsay RCotH> I am getting low on pearls, someone should fill me up!
      }
    }
  }
/return

#Event RCotH:CoHInfo "#1# tell#*#raid,#*#'RCotH> #2#'"
Sub Event_RCotH:CoHInfo(line, chatSender, msg)
  /if (!${Defined[RCotH:Return]}) /declare RCotH:Return string outer
  /call #StripNameLink "${chatSender}" RCotH:Return
  /if (${RCotH:Return.Equal[You]}) /varset RCotH:Return ${Me.Name}
  /varset chatSender ${RCotH:Return}
  /if (${Defined[RCotH:IAMCoHMaster]} && ${msg.Equal[I am available to CoH]}) {
    /if (!${SpawnCount[pc =${chatSender}]}) /return
    /varset RCotH:MageList ${RCotH:MageList}${chatSender}|
  } else /if (${Defined[RCotH:IAMCoHMaster]} && ${msg.Equal[My group is done]}) {
    /call RCotH:MageAvailable ${chatSender}
  } else /if (${msg.Left[17].Equal[Summon your group]} && ${msg.Arg[4, ].Equal[${Me.Name}]}) {
    /call RCotH:CotHMyGroup
  } else /if (${msg.Equal[I am raid leader.]}) {
    /varset RCotH:PreviousLeader ${chatSender}
  }
/return

Sub RCotH:CheckDistances
  /declare i int local
  /for i ${RCotH:RaidManifestStart} to 72
    /if (${RCotH:RaidManifest[${i}].Length}) {
      /call RCotH:NeedsCoH "${RCotH:RaidManifest[${i}]}"
      /if (!${RCotH:Return}) /varset RCotH:RaidManifest[${i}]
    }
  /next i
/return

Sub RCotH:NeedsCoH(name)
  /if (!${SpawnCount[pc =${name}]} || ${Spawn[pc =${name}].Distance}<200) {
    /varset RCotH:Return 0
  } else {
    /varset RCotH:Return 1
  }
/return

Sub RCotH:GetManifestCount
  /declare i int local
  /declare c int local 0
  /for i ${RCotH:RaidManifestStart} to 72
    /if (${RCotH:RaidManifest[${i}].Length}) /varcalc c ${c}+1
  /next i
  /varset RCotH:Return ${c}
/return

Sub RCotH:GetNextFromManifest
  /declare i int local
  /declare emptySoFar int local 1
  /for i ${RCotH:RaidManifestStart} to 72
    /if (!${RCotH:RaidManifest[${i}].Length} && ${emptySoFar}) {
      /varset RCotH:RaidManifestStart ${i}
    } else {
      /varset emptySoFar 0
    }
    /if (${RCotH:RaidManifest[${i}].Length}) {
      /varset RCotH:Return ${RCotH:RaidManifest[${i}]}
      /varset RCotH:RaidManifest[${i}]
      /return
    }
  /next i
/return

Sub RCotH:RemoveFromManifest(Name)
  /declare i int local 1
  /declare emptySoFar int local 1
  /for i ${RCotH:RaidManifestStart} to 72
    /if (!${RCotH:RaidManifest[${i}].Length} && ${emptySoFar}) {
      /varset RCotH:RaidManifestStart ${i}
    } else {
      /varset emptySoFar 0
    }
    /if (${RCotH:RaidManifest[${i}].Equal[${Name}]}) {
      /varset RCotH:RaidManifest[${i}]
      /return
    }
  /next i
/return

Sub RCotH:WriteManifest
  | Soon after this runs, all chars will be dumped into Ungrouped, so we pretend they already are for the manifest
  /declare i int local
  /declare c int local 1
  /for i 1 to 83
    /if (${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2].Length} && !${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2].Equal[empty]}) {
      /varset RCotH:RaidManifest[${c}] ${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2]}
      /varcalc c ${c}+1
    }
  /next i
  /if (!${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].Items}) /return
  /for i 1 to ${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].Items}
    /varset RCotH:RaidManifest[${c}] ${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].List[${i},2]}
    /varcalc c ${c}+1
  /next i
/return

Sub RCotH:SnapshotRaidGroups
  /call #CreateArrays string outer RCotH:RaidSnapshot[72]
  /call #CreateArrays string outer RCotH:RaidUnGroupedSnapshot[${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].Items}]
  /call #CreateVars int outer RCotH:RaidSnapshotSize
  /declare i int local
  /declare c int local 1
  /for i 1 to 83
    /if (${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2].Length} && !${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2].Equal[empty]}) {
      /if (${Math.Calc[(${i}-1) % 7].Int} == 0) {
        /varset RCotH:RaidSnapshot[${c}] ${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2]}|L:${Math.Calc[${Math.Calc[${i}/7].Int}+1].Int}
      } else {
        /varset RCotH:RaidSnapshot[${c}] ${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2]}|G:${Math.Calc[${Math.Calc[${i}/7].Int}+1].Int}
      }
      /varcalc c ${c}+1
    }
  /next i
  /if (${RCotH:RaidUnGroupedSnapshot.Size}) {
    /for i 1 to ${RCotH:RaidUnGroupedSnapshot.Size}
      /varset RCotH:RaidUnGroupedSnapshot[${i}] ${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].List[${i},2]}
    /next i
  }
  /varset RCotH:RaidSnapshotSize ${c}
/return

Sub RCotH:RestoreRaidGroups
  /declare i int local
  /declare j int local
  /for i 1 to 12
    /for j 1 to ${RCotH:RaidSnapshotSize}
      /if (${RCotH:RaidSnapshot[${j}].Length} && ${RCotH:RaidSnapshot[${j}].Arg[2,|].Equal[L:${i}]}) {
        /call RCotH:SelectPlayer "${RCotH:RaidSnapshot[${j}].Arg[1,|]}" RAID_NotInGroupPlayerList "${RCotH:RaidWindowUngroupedIndices}"
        /call RCotH:MoveSelected ${i}
        /varset RCotH:RaidSnapshot[${j}]
        /goto :BreakJ
      }
    /next j
    :BreakJ
  /next i
  /for i 1 to ${RCotH:RaidSnapshotSize} 
    /if (${RCotH:RaidSnapshot[${i}].Length}) {
      /call RCotH:SelectPlayer "${RCotH:RaidSnapshot[${i}].Arg[1,|]}" RAID_NotInGroupPlayerList "${RCotH:RaidWindowUngroupedIndices}"
      /call RCotH:MoveSelected ${RCotH:RaidSnapshot[${i}].Arg[2,|].Arg[2,:]}
      /varset RCotH:RaidSnapshot[${i}]
    }
  /next i
/return

Sub RCotH:SelectByName(name)
  /declare i int local
  /varset RCotH:SelectedPlayer
  /for i 1 to 83
    /if (${Window[RaidWindow].Child[RAID_PlayerList].List[${i},2].Equal[${name}]}) {
      /notify RaidWindow RAID_PlayerList listselect ${i}
      /call RCotH:GetLocationIdentifier RAID_PlayerList ${i}
      /varset RCotH:SelectedPlayer ${name}|RAID_PlayerList|${RCotH:Return}
      /return
    }
  /next i
  /for i 1 to ${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].Items}
    /if (${Window[RaidWindow].Child[RAID_NotInGroupPlayerList].List[${i},2].Equal[${name}]}) {
      /notify RaidWindow RAID_NotInGroupPlayerList listselect ${i}
      /call RCotH:GetLocationIdentifier RAID_NotInGroupPlayerList ${i}
      /varset RCotH:SelectedPlayer ${name}|RAID_NotInGroupPlayerList|${RCotH:Return}
      /return
    }
  /next i
/return

Sub RCotH:GetLocationIdentifier(location, index)
  /declare i int local
  /varset RCotH:Return
  /if (${location.Equal[RAID_PlayerList]}) {
    /if (${Math.Calc[${index} % 7]} ==  0) {
      /varset RCotH:Return Leader:${Math.Calc[${Math.Calc[${index}/7].Int}+1].Int}
    } else {
      /varset RCotH:Return Group:${Math.Calc[${Math.Calc[${index}/7].Int}+1].Int}
    }
  } else /if (${location.Equal[RAID_NotInGroupPlayerList]}) {
    /varset RCotH:Return Ungrouped
  }
/return

Sub RCotH:SelectPlayer(name, location, range)
  /declare i int local
  /varset RCotH:SelectedPlayer
  /for i ${range}
    /if (${Window[RaidWindow].Child[${location}].List[${i},2].Equal[${name}]}) {
      /notify RaidWindow ${location} listselect ${i}
      /call RCotH:GetLocationIdentifier ${location} ${i}
      /varset RCotH:SelectedPlayer ${name}|${location}|${RCotH:Return}
      /delay 4
      /return
    }
  /next i
/return

Sub RCotH:UngroupAll
  /declare i int local 1
  /call RCotH:Lock
  /for i 1 to 12
    :Loop
    /if (${Window[RaidWindow].Child[RAID_PlayerList].List[${Math.Calc[${Math.Calc[((${i}*6-5)/6)].Int}+${i}*6-6+1]},2].NotEqual[empty]}) {
      /notify RaidWindow RAID_PlayerList listselect ${Math.Calc[${Math.Calc[((${i}*6-5)/6)].Int}+${i}*6-6+1]}
      /delay 4
      /notify RaidWindow RAID_NoGroupButton leftmouseup
      /delay 4
      /goto :Loop
    }
  /next i
/return

Sub RCotH:Ungroup(GroupNum)
  /call RCotH:Lock
  :Loop
  /if (${Window[RaidWindow].Child[RAID_PlayerList].List[${Math.Calc[${Math.Calc[((${GroupNum}*6-5)/6)].Int}+${GroupNum}*6-6+1]},2].NotEqual[empty]}) {
    /notify RaidWindow RAID_PlayerList listselect ${Math.Calc[${Math.Calc[((${GroupNum}*6-5)/6)].Int}+${GroupNum}*6-6+1]}
    /delay 4
    /notify RaidWindow RAID_NoGroupButton leftmouseup
    /delay 4
    /goto :Loop
  }
/return

Sub RCotH:UngroupLeaveLeader(GroupNum)
  /call RCotH:Lock
  :Loop
  /if (${Window[RaidWindow].Child[RAID_PlayerList].List[${Math.Calc[${Math.Calc[((${GroupNum}*6-5)/6)].Int}+${GroupNum}*6-6+2]},2].NotEqual[empty]}) {
    /notify RaidWindow RAID_PlayerList listselect ${Math.Calc[${Math.Calc[((${GroupNum}*6-5)/6)].Int}+${GroupNum}*6-6+2]}
    /delay 4
    /notify RaidWindow RAID_NoGroupButton leftmouseup
    /delay 4
    /goto :Loop
  }
/return

Sub RCotH:MoveSelected(Destination)
  /if (${Select[${Destination},NoGroup,Ungrouped]}) {
    /notify RaidWindow RAID_NoGroupButton leftmouseup
  } else {
    /notify RaidWindow RAID_Group${Destination}Button leftmouseup
  }
  /delay 4
/return

Sub RCotH:WaitForLeadership
  /delay 200 ${Raid.Leader.Name.Equal[${Me.Name}]}
/return

Sub RCotH:MakeLeader(NewLeader)
  /if (!${Raid.Leader.Name.Equal[${Me.Name}]}) /return
  /call RCotH:OpenWindow
  /call RCotH:Unlock
  /makeraidleader ${NewLeader}
  /call RCotH:CloseWindow
/return

Sub RCotH:Unlock
  /notify RaidWindow RAID_UnlockButton leftmouseup
  /delay 5
/return

Sub RCotH:Lock
  /notify RaidWindow RAID_LockButton leftmouseup
  /delay 5
/return

Sub RCotH:OpenWindow
  /windowstate RaidWindow open
  /delay 10 ${Window[RaidWindow].Open}
/return

Sub RCotH:CloseWindow
  /windowstate RaidWindow close
  /delay 10 !${Window[RaidWindow].Open}
/return
