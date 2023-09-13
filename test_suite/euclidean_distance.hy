class SortingUtils is

    // Bubble Sort
    method bubbleSort(arr: Array[Integer]) is
        var n: Integer := arr.Length
        var swapped: Boolean := true
        while swapped loop
            swapped := false
            var i: Integer := 1
            while i.Less(n) loop
                if arr.get(i).GreaterThan(arr.get(i.Plus(1))) then
                    // Swap arr[i] and arr[i+1]
                    var temp: Integer := arr.get(i)
                    arr.set(i, arr.get(i.Plus(1)))
                    arr.set(i.Plus(1), temp)
                    swapped := true
                end
                i := i.Plus(1)
            end
        end
    end

    // Selection Sort
    method selectionSort(arr: Array[Integer]) is
        var n: Integer := arr.Length
        var i: Integer := 1
        while i.Less(n) loop
            var minIndex: Integer := i
            var j: Integer := i.Plus(1)
            while j.LessEqual(n) loop
                if arr.get(j).Less(arr.get(minIndex)) then
                    minIndex := j
                end
                j := j.Plus(1)
            end
            // Swap arr[i] and arr[minIndex]
            var temp: Integer := arr.get(i)
            arr.set(i, arr.get(minIndex))
            arr.set(minIndex, temp)
            i := i.Plus(1)
        end
    end

    // Insertion Sort
    method insertionSort(arr: Array[Integer]) is
        var n: Integer := arr.Length
        var i: Integer := 2
        while i.LessEqual(n) loop
            var key: Integer := arr.get(i)
            var j: Integer := i.Minus(1)
            while j.Greater(0) and arr.get(j).GreaterThan(key) loop
                arr.set(j.Plus(1), arr.get(j))
                j := j.Minus(1)
            end
            arr.set(j.Plus(1), key)
            i := i.Plus(1)
        end
    end

    // Quick Sort
    method quickSort(arr: Array[Integer], low: Integer, high: Integer) is
        if low.Less(high) then
            var pivotIndex: Integer := partition(arr, low, high)
            quickSort(arr, low, pivotIndex.Minus(1))
            quickSort(arr, pivotIndex.Plus(1), high)
        end
    end

    method partition(arr: Array[Integer], low: Integer, high: Integer) : Integer is
        var pivot: Integer := arr.get(high)
        var i: Integer := low.Minus(1)
        var j: Integer := low
        while j.Less(high) loop
            if arr.get(j).LessEqual(pivot) then
                i := i.Plus(1)
                // Swap arr[i] and arr[j]
                var temp: Integer := arr.get(i)
                arr.set(i, arr.get(j))
                arr.set(j, temp)
            end
            j := j.Plus(1)
        end
        // Swap arr[i+1] and arr[high]
        var temp: Integer := arr.get(i.Plus(1))
        arr.set(i.Plus(1), arr.get(high))
        arr.set(high, temp)
        return i.Plus(1)
    end

end
