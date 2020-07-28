project "Engine"
	defineProject("ConsoleApp", true, "Engine")
	dependson("Reflection")
	dependson("Utils")
	dependson("Rendering")
	includeModule("Tools/Reflection", "Reflection")
	includeModule("Core/Utils", "Utils")
	includeModule("Core/Rendering", "Rendering")