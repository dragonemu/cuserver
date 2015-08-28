LegSweepCommand = {
        name = "legsweep",

	damageMultiplier = 0.8,
	speedMultiplier = 1.0,

	actionCostMultiplier = 1.4,
	mindCostMultiplier = 0.2,

	animationCRC = hashCode("combo_3d_medium"),
	combatSpam = "legsweep",

	stateEffects = {
	  StateEffect( 
		KNOCKDOWN_EFFECT, 
		{ "knockdownRecovery", "lastKnockdown" }, 
		{ "knockdown_defense" }, 
		{}, 
		70, 
		100, 
		0 
	  )
	},

	range = -1
}

AddCommand(LegSweepCommand)
