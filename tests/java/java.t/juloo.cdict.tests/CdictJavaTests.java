package juloo.cdict.tests;

import java.nio.file.Files;
import java.nio.file.FileSystems;
import juloo.cdict.Cdict;

public class CdictJavaTests
{
  static Cdict open_dict(String fname) throws Exception
  {
    byte[] data = Files.readAllBytes(FileSystems.getDefault().getPath(fname));
    Cdict[] dicts = Cdict.of_bytes(data);
    for (Cdict d : dicts)
      if (d.name.equals("main"))
        return d;
    throw new Exception("No dictionary named 'main'.");
  }

  static void query_distance(Cdict dict, int dist, String word)
  {
    for (int idx : dict.distance(word, dist, 5))
      System.out.printf("close match: %s distance=%d freq=%d\n",
          dict.word(idx), dist, dict.freq(idx));
  }

  static void query_correct(Cdict dict, String word)
  {
    int[] indexes = new int[5];
    int[] costs = new int[5];
    int n = dict.correct(word, null, 4, 4, indexes, costs);
    for (int i = 0; i < n; i++)
      System.out.printf("correction: %s cost=%d freq=%d\n",
          dict.word(indexes[i]), costs[i], dict.freq(indexes[i]));
  }

  static void query(Cdict dict, String word)
  {
    Cdict.Result r = dict.find(word);
    if (r.found)
      System.out.printf("found: %s freq=%d index=%d\n", word,
          dict.freq(r.index), r.index);
    else
      System.out.printf("not found: %s\n", word);
    for (int idx : dict.suffixes(r, 5))
      System.out.printf("prefix: %s freq=%d index=%d\n",
          dict.word(idx), dict.freq(idx), idx);
    query_distance(dict, 1, word);
    query_distance(dict, 2, word);
    query_correct(dict, word);
  }

  public static void main(String[] args) throws Exception
  {
    Cdict dict = open_dict(args[1]);
    for (int i = 2; i < args.length; i++)
      query(dict, args[i]);
  }
}
