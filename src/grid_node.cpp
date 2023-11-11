#include "grid_node.h"

#define DIFFUSIVITY 0.15e-6
#define PIXEL_LENGTH 1/(1500.0 / SCREEN_SCALE); //m/px
#define PIXEL_VOLUME PIXEL_LENGTH * PIXEL_LENGTH * PIXEL_LENGTH //m^3/px^3
#define SPECIFIC_HEAT_CAPACITY 2.5 //kJ/(kg*K)
#define MELTING_POINT 55 //C
#define BOILING_POINT 370 //C
#define LATENT_HEAT 176 //kJ/kg
#define ACTUAL_MELTING_POINT MELTING_POINT + LATENT_HEAT / SPECIFIC_HEAT_CAPACITY //C
#define DENSITY 900 //kg/m^3
#define PIXEL_MASS PIXEL_VOLUME * DENSITY //kg/px^3 / kg
#define SPECIFIC_HEAT_CAPACITY_MASS SPECIFIC_HEAT_CAPACITY * PIXEL_MASS //kJ/K
#define LIQUID_MOVE_TIME 1.0/(1.0 * SCREEN_SCALE) //s/px

using namespace Simulation;

void GridNode::update() {
	switch (state) {
	case SOLID:
		break;
	case LIQUID:
		break;
	case GAS:
		break;
	}
}
