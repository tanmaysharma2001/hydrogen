method BinaryExponentiation(base: Real, exponent: Integer): Real is
    var result := Real(1)
    while exponent.Greater(Integer(0)) loop
        if exponent.Rem(Integer(2)).Equal(Integer(1)) then
            result := result.Mult(base)
        end
        base := base.Mult(base)
        exponent := exponent.Div(Integer(2))
    end
    return result
end
