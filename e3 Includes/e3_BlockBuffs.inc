|------------------------------------------------------------------------|
|- e3_BlockBuffs.inc v1.0    originally designed to work with e3.mac )  -|
|- Written by Athrogate of PEQ                                          -|
|------------------------------------------------------------------------|
|- Contains commands to run to block spells for classes, archtypes and  -|
|- Characters.                                                          -|
|------------------------------------------------------------------------|

sub BlockBuffsAtStartup

  /declare classesMelee string local BRD,BER,MNK,ROG,WAR,BRD
  /declare classesCaster string local WIZ,MAG,NEC,ENC
  /declare classesPriest string local CLR,DRU,SHM
  /declare classesHybrid string local PAL,SHD,BST

  /call BlockBuffSettingReader

  /if (${Bool[${Select[${Me.Class.ShortName}, ${classesMelee}]}]}) {
    /call #CombineArrays myBlockedBuffs blockBuffsMelee blockBuffs${Me.Class.Name} blockBuffsMine
  }
  /if (${Bool[${Select[${Me.Class.ShortName}, ${classesCaster}]}]}) {
    /call #CombineArrays myBlockedBuffs blockBuffsCaster blockBuffs${Me.Class.Name} blockBuffsMine
  }
  /if (${Bool[${Select[${Me.Class.ShortName}, ${classesPriest}]}]}) {
    /call #CombineArrays myBlockedBuffs blockBuffsPriest blockBuffs${Me.Class.Name} blockBuffsMine
  }
  /if (${Bool[${Select[${Me.Class.ShortName}, ${classesHybrid}]}]}) {
    /call #CombineArrays myBlockedBuffs blockBuffsHybrid blockBuffs${Me.Class.Name} blockBuffsMine
  }
  /squelch /blockspell clear me
  /declare i int local 0
  /for i 1 to ${myBlockedBuffs.Size}
    |/echo ${myBlockedBuffs[${i}]}: ${Spell[${myBlockedBuffs[${i}]}].ID}
    /squelch /blockspell add me ${Spell[${myBlockedBuffs[${i}]}].ID}
  /next i
/return

sub BlockBuffSettingReader

  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Melee,Block#" blockBuffsMelee
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Priest,Block#" blockBuffsPriest
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Caster,Block#" blockBuffsCaster
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Hybrid,Block#" blockBuffsHybrid

  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Bard,Block#" blockBuffsBard
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Beserker,Block#" blockBuffsBeserker
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Beastlord,Block#" blockBuffsBeastlord
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Enchanter,Block#" blockBuffsEnchanter
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Cleric,Block#" blockBuffsCleric
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Druid,Block#" blockBuffsDruid
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Magician,Block#" blockBuffsMagician
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Monk,Block#" blockBuffsMonk
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Necromancer,Block#" blockBuffsNecromancer
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Paladin,Block#" blockBuffsPaladin
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Ranger,Block#" blockBuffsRanger
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Rogue,Block#" blockBuffsRogue
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Shadowknight,Block#" blockBuffsShadowknight
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Shaman,Block#" blockBuffsShaman
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Warrior,Block#" blockBuffsWarrior
  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,Wizard,Block#" blockBuffsWizard

  /call #IniToArray "e3 Macro Inis\BlockBuffs.ini,${Me.Name},Block#" blockBuffsMine

/return