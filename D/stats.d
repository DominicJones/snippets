// dmd -wi -d stats.d && echo 3 5 1.3 4 10 4.5 1 5 | ./stats Min Average

import std.exception;
import std.stdio;


interface Stat {
   void accumulate(double x);
   void postprocess();
   double result();
}


abstract class IncrementalStat : Stat {
   protected double _result;
   void accumulate(double x);
   void postprocess() {}
   double result() {
      return _result;
   }
}


class Min : IncrementalStat {
   this() {
      _result = double.max;
   }
   void accumulate(double x) {
      if (x < _result) {
         _result = x;
      }
   }
}


class Average : IncrementalStat {
   private uint items = 0;
   this() {
      _result = 0;
   }
   void accumulate(double x) {
      _result += x;
      ++items;
   }
   override void postprocess() {
      if (items) {
         _result /= items;
      }
   }
}


void main(string[] args) {
   Stat[] stats;

   foreach (arg; args[1 .. $]) {
      auto newStat = cast(Stat) Object.factory("stats." ~ arg);
      enforce(newStat, "Invalid statistics function: " ~ arg);
      stats ~= newStat;
   }

   for (double x; readf(" %f ", &x) == 1; ) {
      foreach (s; stats) {
         s.accumulate(x);
      }
   }

   foreach (s; stats) {
      s.postprocess();
      writeln(s.result());
   }
}
