project "msdfgen"
	language    "C++"
	kind        "StaticLib"
    includedirs {
        "./",
		"../libschrift"
	}
	files
	{
		"*.cpp",
		"core/*.cpp",
		"../libschrift/schrift.c"
	}