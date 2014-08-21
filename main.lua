print("LUA SCRIPTHAS BEEN OPEN");
times = 0

function lua_main()
	if times > 60 then
		times = 0;
		print("--------");	
	end
	times = times+1;
end