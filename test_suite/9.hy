class vehicle is
	var velocity : Real
	var max_velocity : Real
	
	var acceleration : Real

	this (ms : Real) is
		velocity := 0
		max_velocity := ms
		acceleration := 0
	end

	method velocity_up (acc : Real) is
		while velocity.Less(max_velocity) loop
			velocity := velocity.Plus(acc)
		end
		velocity := max_velocity
	end

	method velocity_up (acc : Real, till : Real) is
		till := till.Minus(velocity)

		while till.Greater(0) loop
			velocity := velocity.Plus(acc)
			till := till.Minus(acc)
		end
	end

end