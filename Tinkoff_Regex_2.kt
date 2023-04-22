fun main() = System.`in`.bufferedReader().useLines { lines ->
    lines.map { s ->
        s.replace(Regex("\\\\circle\\{\\((\\d+),(\\d+)\\)(.*)}")) { result ->
            "\\circle{(${result.groupValues[2]},${result.groupValues[1]})${result.groupValues[3]}}"
        }
    }.forEach(::println)
}
