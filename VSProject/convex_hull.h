#pragma once

#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <Godot.hpp>
#include <Sprite.hpp>
#include "QuickHullCPU.h"
#include "QuickHullGPU.h"
#include "MergeHull.h"
#include <chrono>
#include <fstream>

namespace godot {

	class ConvexHullCPP : public Sprite {
		GODOT_CLASS(ConvexHullCPP, Sprite)

	private:
			QuickHullGPU quick_hull_gpu;

	public:
		static void _register_methods();

		ConvexHullCPP();
		~ConvexHullCPP();

		void _init(); // our initializer called by Godot

		Array compute_hull(Array points, godot_int d_size, godot_bool step);
	};

} // namespace godot

#endif