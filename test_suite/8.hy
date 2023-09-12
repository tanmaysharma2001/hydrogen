class math_formules is 
	method factorial (num : Integer) : Integer is
		var increment := 1
		var result := 1

		while increment.LessEqual(num) loop
			result := result.Mult(increment)
			increment := increment.Plus(increment)
		end

		return result
	end
end