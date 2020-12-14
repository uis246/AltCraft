local plugin = {
	modid = 'altcraft',
	onLoad = nil,
	onUnload = nil,
	onChangeState = nil,
	onTick = nil,
	onRequestBlockInfo = nil,
}

function plugin.onLoad ()
	print("Loaded AltCraft plugin!")
end

function plugin.onChangeState (newState)
	AC.LogWarning("New state: "..newState)
end

function plugin.onUnload ()
	AC.LogInfo("AC Core unloaded")
end

local blocks = require("altcraft/blocks")
blocks.RegisterBlocks()

function plugin.onRequestBlockInfo(blockPos)
	return blocks.GetBlockInfo(blockPos)
end

AC.RegisterDimension(0, Dimension.new("overworld", true))
AC.RegisterDimension(-1, Dimension.new("the_nether", false))
AC.RegisterDimension(1, Dimension.new("the_end", false))

AC.RegisterPlugin(plugin)
plugin = nil
