/*
    Clocktest, a simple program to test POSIX timing functions
    Copyright (C) 2011  Janne Blomqvist

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <float.h>

static const int itr = 100;

void test_clock_gettime(clockid_t clk_id, const char *name)
{
  struct timespec ts1, ts2;
  double min, max, avg, ncavg, cur;
  int ncalls = 0;
  int i;
  avg = max = 0;
  min = DBL_MAX;

  clock_getres(clk_id, &ts1);
  printf("Resolution of %s: %ld s, %ld ns.\n", name, ts1.tv_sec, ts1.tv_nsec);

  printf("Testing clock_gettime() with %s\n", name);
  for (i = 0; i < itr; i++)
    {
      clock_gettime(clk_id, &ts1);
      ts2 = ts1;
      while ((ts1.tv_sec == ts2.tv_sec) && (ts1.tv_nsec == ts2.tv_nsec))
        {
          clock_gettime(clk_id, &ts2);
          ncalls++;
        }
      cur = (ts2.tv_sec - ts1.tv_sec) * 1000000000.0;
      cur += (ts2.tv_nsec - ts1.tv_nsec);
      if (cur < min)
	min = cur;
      if (cur > max)
	max = cur;
      avg += cur;
    }
  ncavg = (double) ncalls / itr;
  avg /= itr;
  printf("Clock avg/min/max: %g/%g/%g ns.\n", avg, min, max);
  printf("On average %g clock_gettime() calls before value changed.\n", ncavg);
  printf("Thus, %g clock_gettime() calls per second.\n\n", 
         ncavg / avg * 1000000000);
}

void test_time()
{
  /* Since time has only 1s resolution, don't iterate many (itr) times. */
  const int timeitr = 5;
  time_t t1, t2, cur;
  double min, max, avg, ncavg;
  long ncalls = 0;
  int i;

  printf("Testing time()\n");
  max = avg = 0;
  min = DBL_MAX;
  for (i = 0; i < timeitr; i++)
    {
      t1 = t2 = time(NULL);
      while (t1 == t2)
        {
          t2 = time(NULL);
          ncalls++;
        }
      cur = t2 - t1;
      if (cur < min)
        min = cur;
      if (cur > max)
        max = cur;
      avg += cur;
    }
  avg /= timeitr;
  ncavg = (double) ncalls / timeitr;
  printf("avg/min/max: %g/%g/%g s.\n", avg, min, max);
  printf("On average %g time() calls before value changed.\n", ncavg);
  printf("Thus, %g time() calls per second.\n\n", ncavg / avg);
}


void test_clock()
{
  double min, max, avg, cur, ncavg;
  long ncalls;
  clock_t t1, t2;
  int i;

  printf("Testing clock()\n");
  max = 0;
  min = DBL_MAX;
  avg = 0;
  ncalls = 0;
  for (i = 0; i < itr; i++)
    {
      t1 = t2 = clock();

      while(t1 == t2)
        {
          t2 = clock();
          ncalls++;
        }
	
      cur = (double)(t2 - t1) / CLOCKS_PER_SEC * 1000;
      if (cur < min)
        min = cur;
      if (cur > max)
        max = cur;
      avg += cur;
    }
  avg /= itr;
  ncavg = (double) ncalls / itr;
  printf("avg/min/max: %g/%g/%g ms.\n", avg, min, max);
  printf("On average %g clock() calls before value changed.\n", ncavg);
  printf("Thus, %g clock() calls per second.\n", ncavg / avg * 1000);
}

void test_times()
{
  struct tms tm1, tm2;
  double maxr, maxu, maxs, avgr, avgu, avgs, minr, minu, mins, curr, curu, 
    curs, ncavg;
  long ncalls;
  clock_t t1, t2;
  int i;
  long clk_tck = sysconf(_SC_CLK_TCK);

  printf("\nTesting times()\n");
  ncalls = 0;
  printf("_SC_CLK_TCK: %ld\n", clk_tck);
  maxr = maxu = maxs = avgr = avgu = avgs = 0;
  minr = minu = mins = DBL_MAX;
  for (i = 0; i < itr; i++)
    {
      t1 = t2 = times(&tm1);
      tm2 = tm1;
      while ((t1 == t2) && (tm1.tms_utime == tm2.tms_utime) 
             && (tm1.tms_stime == tm2.tms_stime))
        {
          t2 = times (&tm2);
          ncalls++;
        }
      curr = (double)(t2 -t1) / clk_tck * 1000;
      curu = (double)(tm2.tms_utime - tm1.tms_utime) / clk_tck * 1000;
      curs = (double)(tm2.tms_stime - tm1.tms_stime) / clk_tck * 1000;
      if (curu < minu)
        minu = curu;
      if (curr < minr)
        minr = curr;
      if (curs < mins)
        mins = curs;
      if (curr > maxr)
        maxr = curr;
      if (curu > maxu)
        maxu = curu;
      if (curs > maxs)
        maxs = curs;
      avgr += curr;
      avgu += curu;
      avgs += curs;
    }
  avgr /= itr;
  avgu /= itr;
  avgs /= itr;
  ncavg = (double) ncalls / itr;
  printf("Real time Avg/Min/Max: %g/%g/%g ms.\n", avgr, minr, maxr);
  printf("User time Avg/Min/Max: %g/%g/%g ms.\n", avgu, minu, maxu);
  printf("Sys time Avg/Min/Max: %g/%g/%g ms.\n", avgs, mins, maxs);
  printf("On average %g times() calls before value changed.\n", ncavg);
  printf("Thus, %g times() calls per second.\n", ncavg / avgr * 1000);
}

void test_getrusage()
{
  struct rusage r1, r2;
  double minr, minu, mins, maxr, maxu, maxs, avgr, avgu, avgs, curu, curs, ncavg;
  long ncalls;
  int i;

  printf("\nTesting getrusage()\n");
  ncalls = 0;

  maxr = maxu = maxs = avgr = avgu = avgs = 0;
  minr = minu = mins = DBL_MAX;
  for (i = 0; i < itr; i++)
    {
      getrusage(RUSAGE_SELF, &r1);
      r2 = r1;
      while ((r2.ru_utime.tv_sec == r1.ru_utime.tv_sec)
             && (r2.ru_utime.tv_usec == r1.ru_utime.tv_usec)
             && (r2.ru_stime.tv_sec == r1.ru_stime.tv_sec)
             && (r2.ru_stime.tv_usec == r1.ru_stime.tv_usec))
        {
          getrusage(RUSAGE_SELF, &r2);
          ncalls++;
        }
      curu = (r2.ru_utime.tv_sec - r1.ru_utime.tv_sec) * 1000000;
      curu += (r2.ru_utime.tv_usec - r1.ru_utime.tv_usec);
      curs = (r2.ru_stime.tv_sec - r1.ru_stime.tv_sec) * 1000000;
      curs += (r2.ru_stime.tv_usec - r1.ru_stime.tv_usec);
      if (curu < minu)
        minu = curu;
      if (curs < mins)
        mins = curs;
      if (curu > maxu)
        maxu = curu;
      if (curs > maxs)
        maxs = curs;
      avgu += curu;
      avgs += curs;
    }
  avgu /= itr;
  avgs /= itr;
  ncavg = (double) ncalls / itr;
  printf("User time Avg/min/max: %g/%g/%g us.\n", avgu, minu, maxu);
  printf("System time avg/min/max: %g/%g/%g us.\n", avgs, mins, maxs);
  printf("On average %g getrusage() calls before value changed.\n", ncavg);
  printf("Thus, %g getrusage() calls per second.\n", ncavg / (avgu + avgs) * 1000000);
}


void test_gettimeofday()
{
  struct timeval tv1, tv2;
  double min, max, avg, ncavg, cur;
  long ncalls = 0;
  int i;

  printf("\nTesting gettimeofday()\n");
  ncalls = 0;

  avg = max = 0;
  min = DBL_MAX;
  for (i = 0; i < itr; i++)
    {
      gettimeofday(&tv1, NULL);
      tv2 = tv1;
      while ((tv1.tv_sec == tv2.tv_sec) && (tv1.tv_usec == tv2.tv_usec))
        {
          gettimeofday(&tv2, NULL);
          ncalls++;
        }
      cur = (tv2.tv_sec - tv1.tv_sec) * 1000000.0;
      cur += (tv2.tv_usec - tv1.tv_usec);
      if (cur < min)
        min = cur;
      if (cur > max)
        max = cur;
      avg += cur;
    }
  avg /= itr;
  ncavg = (double) ncalls / itr;
  printf("Wall time avg/min/max: %g/%g/%g us.\n", avg, min, max);
  printf("On average %g gettimeofday() calls before value changed.\n", ncavg);
  printf("Thus, %g gettimeofday() calls per second.\n", ncavg / avg * 1000000);
}


int main()
{
  test_time();
  test_clock();
  test_times();
  test_getrusage();
  test_gettimeofday();

  printf("\nHigh precision clock_gettime() tests:\n\n");

  test_clock_gettime(CLOCK_REALTIME, "CLOCK_REALTIME");
#ifdef CLOCK_MONOTONIC
  test_clock_gettime(CLOCK_MONOTONIC, "CLOCK_MONOTONIC");
#endif
#ifdef CLOCK_PROCESS_CPUTIME_ID
  test_clock_gettime(CLOCK_PROCESS_CPUTIME_ID, "CLOCK_PROCESS_CPUTIME_ID");
#endif
#ifdef CLOCK_THREAD_CPUTIME_ID
  test_clock_gettime(CLOCK_THREAD_CPUTIME_ID, "CLOCK_THREAD_CPUTIME_ID");
#endif

  return 0;
}
