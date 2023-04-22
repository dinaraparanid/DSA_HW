import java.util.Locale

fun main() {
    val s = readln()
    val trash = s.split("\\w+").toMutableList()
    val lowercaseWords = s.lowercase(Locale.getDefault()).split(Regex("\\W+"))

    val isStartsWithTrash = s.matches(Regex("$\\W+"))

    val uppercaseWordsIndices = s
        .split(Regex("\\W+"))
        .asSequence()
        .filter(String::isNotEmpty)
        .mapIndexed { ind, x -> ind to x }
        .filter { (_, x) -> x[0].isUpperCase() }
        .map { (ind, _) -> ind }
        .toHashSet()

    val articlesRgx = Regex("^(the|a|an)$")

    val articlesIndices = lowercaseWords
        .asSequence()
        .mapIndexed { ind, x -> ind to x }
        .filter { (_, x) -> x.matches(articlesRgx) }
        .map { (ind, _) -> ind }
        .toHashSet()

    val digitsLettersStrings = lowercaseWords
        .asSequence()
        .map { str ->
            str.replace(Regex("c([^iek])")) { "k${it.groupValues[1]}" }
                .replace("ck", "k")
                .replace(Regex("c([ie])")) { "s${it.groupValues[1]}" }
        }
        .map { str ->
            str.replace("ee", "i")
                .replace("oo", "u")
                .replace(Regex("([a-z])\\1")) { it.groupValues[1] }
        }
        .map { str ->
            str.count { it == 'e' }
                .takeIf { it > 1 && str.endsWith('e') }
                ?.let { str.dropLast(1) } ?: str
        }
        .filter(String::isNotEmpty)
        .zip(generateSequence(0) { it + 1 })
        .filter { (_, ind) -> ind !in articlesIndices  }
        .map { (str, ind) -> ind to (if (ind in uppercaseWordsIndices) str.replaceFirstChar(Char::uppercaseChar) else str) }
        .toMutableList()

    print(if (isStartsWithTrash) trash.removeFirst() else digitsLettersStrings.removeFirst().first)

    trash
        .mapIndexed { ind, str -> ind to str }
        .toMap()
        .toSortedMap()
        .apply { putAll(digitsLettersStrings) }
        .values
        .joinToString()
        .let(::print)
}
