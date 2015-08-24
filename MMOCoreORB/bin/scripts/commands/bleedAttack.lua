BleedAttackCommand = {
        name = "bleedattack",

	damageMultiplier = 1.5,
	speedMultiplier = 2.0,

	actionCostMultiplier = 1.4,
	mindCostMultiplier = 0.2,

	animationCRC = hashCode("combo_3c_light"),

	combatSpam = "shiimshiak",

	dotEffects = {
	  DotEffect( 
		BLEEDING, 
		{ "resistance_bleeding", "bleed_resist" },
		HEALTH,
		true,
		200,
		100,
		60, 
		30
	  )
	},

	range = -1
}

AddCommand(BleedAttackCommand)

