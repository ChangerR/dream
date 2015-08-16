print("LUA SCRIPTHAS BEEN OPEN");
times = 0

function lua_main()
	if times > 1000 then
		times = 0;
		print("lua script run");	
	end
	times = times+1;
end