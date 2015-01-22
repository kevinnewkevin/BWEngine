--
-- Author: Bai Yun
-- Date: 2014-12-11 21:05:05
--

MEEvent = {
	TOUCH_BEGAN = 1,
	TOUCH_MOVED = 2,
	TOUCH_ENDED = 3,
	TOUCH_CANCELLED = 4,

	CLICK = 5,

	ENTER = 6,
	EXIT = 7,
	CLEANUP = 8,

	ENTER_FRAME = 9
}

local node__clickOffset = 30

function cc.Node:node__hitTest(pos)
	local rect = self:getBoundingBox()
	return cc.rectContainsPoint( rect, pos )
end

function cc.Node:node__handleEvent(nType, ...)
	local handle = self.node__eventList[nType]
	if handle then 
		handle(self, ...)
	end
end

function cc.Node:node__initEvent()
	self.node__eventList = self.node__eventList or {}
	if self.node_isTouchEnabled then 
		if not self.node_listener then 
			local touchNode = cc.Node:create()
			touchNode:setContentSize(self:getContentSize())
			self:addChild(touchNode, 100000)
			touchNode:setAnchorPoint(self:getAnchorPoint())
			self.node__touchNode = touchNode

		    local listener = cc.EventListenerTouchOneByOne:create()
		    local touchBeganPosition
		    self.node__eventList.touchBegan = function(touch, event)
		    	local pos = touch:getLocation()
		    	if self:node__hitTest(pos) then 
		    		touchBeganPosition = pos
		    		self:node__handleEvent(MEEvent.TOUCH_BEGAN, pos)
		    		return true
		    	end
		    	return false
			end
		    self.node__eventList.touchMoved = function(touch, event)
		    	local pos = touch:getLocation()
		    	local delta = touch:getDelta()
		    	self:node__handleEvent(MEEvent.TOUCH_MOVED, pos, delta)
			end
		    self.node__eventList.touchEnded = function(touch, event)
		    	local pos = touch:getLocation()
		    	self:node__handleEvent(MEEvent.TOUCH_ENDED, pos)
		    	if touchBeganPosition and node__clickOffset >= cc.pGetLength(cc.pSub(pos, touchBeganPosition)) then 
		    		self:node__handleEvent(MEEvent.CLICK, pos)
		    	end
		    	touchBeganPosition = nil
			end
		    self.node__eventList.touchCancelled = function(touch, event)
		    	local pos = touch:getLocation()
		    	self:node__handleEvent(MEEvent.TOUCH_CANCELLED, pos)
		    	touchBeganPosition = nil
			end

			if not self.setTouchEnabled then
				listener:setSwallowTouches(true)
			end
		    listener:registerScriptHandler(self.node__eventList.touchBegan, cc.Handler.EVENT_TOUCH_BEGAN)
		    listener:registerScriptHandler(self.node__eventList.touchMoved, cc.Handler.EVENT_TOUCH_MOVED)
		    listener:registerScriptHandler(self.node__eventList.touchEnded, cc.Handler.EVENT_TOUCH_ENDED)
		    listener:registerScriptHandler(self.node__eventList.touchCancelled, cc.Handler.EVENT_TOUCH_CANCELLED)
		    local eventDispatcher = touchNode:getEventDispatcher()
		    eventDispatcher:addEventListenerWithSceneGraphPriority(listener, touchNode)

		    self.node_listener = listener
		end
	else 
		if self.node_listener and not tolua.isnull(self.node_listener) then 
			local eventDispatcher = self:getEventDispatcher()
			eventDispatcher:removeEventListener(self.node_listener)
			self.node_listener = nil
		end
	end

	if not self.node__isInitNodeEvents then 
		local eventDispatcher = self:getEventDispatcher()
	    local function onNodeEvent(event)
	        if event == "enter" then
	        	self:node__handleEvent(MEEvent.ENTER)
	        elseif event == "exit" then
	        	self:node__handleEvent(MEEvent.EXIT)
	        elseif event == "cleanup" then
	        	self:node__handleEvent(MEEvent.CLEANUP)
	        	eventDispatcher:removeEventListener(self.node_listener)
				self.node_listener = nil
	        end
	    end
    	self:registerScriptHandler(onNodeEvent)
    	self.node__isInitNodeEvents = true
    end
end

function cc.Node:addMEListener(nType, handle)
	self:node__initEvent()
	self.node__eventList[nType] = handle

	if nType == MEEvent.ENTER_FRAME then 
		self:scheduleUpdateWithPriorityLua(function(dt)
			self:node__handleEvent(MEEvent.ENTER_FRAME, dt)
		end, 0)
	end
end

function cc.Node:removeMEListener(nType)
	self:node__initEvent()
	self.node__eventList[nType] = nil
	-- if nType == MEEvent.ENTER_FRAME then 
	-- 	self:unscheduleUpdate()
	-- end
end

function cc.Node:setMETouchEnabled(enable)
	self:node__initEvent()
	if self.node_isTouchEnabled ~= enable then 
		self.node_isTouchEnabled = enable
		self:node__initEvent()
	end
end

function cc.Node:isMETouchEnabled()
	return self.node_isTouchEnabled
end

function cc.Node:setMEClickOffset(offset)
	node__clickOffset = offset
end

function cc.Node:getMEClickOffset()
	return node__clickOffset
end

