method EuclideanDistance(x1: Real, y1: Real, x2: Real, y2: Real): Real is
    var dx := x2.Minus(x1)
    var dy := y2.Minus(y1)
    return (dx.Mult(dx).Plus(dy.Mult(dy))).Sqrt()
end
