#include <stdio.h>
#include <stdlib.h>

#include "GenBDTree.h"
#include "CardCluster.h"
#include "CardAbst.h"
#include "DiskFiles.h"
#include "GameInfo.h"
#include "FileOperation.h"

CardCluster::CardCluster(const CardAbst &ca, bool numb_only)
{
  GenBDTree::Create();
  int max_street = GameInfo::MaxStreet();
  none_.reset(new bool[max_street + 1]);
  short_buckets_ = new unsigned short *[max_street + 1];
  int_buckets_ = new int *[max_street + 1];
  for (int st = 0; st <= max_street; ++st)
  {
    short_buckets_[st] = nullptr;
    int_buckets_[st] = nullptr;
  }
  char buf[500];
  num_buckets_.reset(new int[max_street + 1]);
  for (int st = 0; st <= max_street; ++st)
  {
    if (ca.Bucketing(st) == "none")
    {
      none_[st] = true;
      num_buckets_[st] = 0;
      printf("None Bucket Occur!\n");
      continue;
    }
    none_[st] = false;
    sprintf(buf, "%s/num_buckets.%s.%i.%i.%i.%s.%i", DiskFiles::GetstaticDirPath(), GameInfo::GameName().c_str(),
            GameInfo::NumRanks(), GameInfo::NumSuits(), max_street, ca.Bucketing(st).c_str(), st);
    Reader reader(buf);
    num_buckets_[st] = reader.ReadIntOrQ();  
  }

  if (!numb_only)
  {
    for (int st = 0; st <= max_street; ++st)
    {
      if (none_[st])
        continue;
      int num_boards = GenBDTree::NumBoards(st);
      int num_hole_card_pairs = GameInfo::NumHoleCardPairs(st);

      unsigned int num_hands = (unsigned int)num_boards * (unsigned int)num_hole_card_pairs;
      long long int lli_num_hands = num_hands;  

      sprintf(buf, "%s/buckets.%s.%i.%i.%i.%s.%i", DiskFiles::GetstaticDirPath(), GameInfo::GameName().c_str(),
              GameInfo::NumRanks(), GameInfo::NumSuits(), max_street, ca.Bucketing(st).c_str(), st);
      Reader reader(buf);
      long long int file_size = reader.FileSize();
      if (file_size == lli_num_hands * 2)
      {       
        short_buckets_[st] = new unsigned short[num_hands];
        for (unsigned int h = 0; h < num_hands; ++h)
        {
          short_buckets_[st][h] = reader.ReadUnsignedShortOrQ();
        }
      }
      else if (file_size == lli_num_hands * 4)
      {       
        int_buckets_[st] = new int[num_hands];
        for (unsigned int h = 0; h < num_hands; ++h)
        {
          int_buckets_[st][h] = reader.ReadIntOrQ();
        }
      }
      else
      {
        fprintf(stderr, "BucketsInstance::Initialize: Unexpected file size %lli\n", file_size);
        exit(-1);
      }
    }
  }
}

CardCluster::CardCluster(void)
{
  short_buckets_ = nullptr;
  int_buckets_ = nullptr;
}

CardCluster::~CardCluster(void)
{
  int max_street = GameInfo::MaxStreet();
  if (short_buckets_)
  {
    for (int st = 0; st <= max_street; ++st)
    {
      if (short_buckets_[st])
        delete[] short_buckets_[st];
    }
    delete[] short_buckets_;
  }
  if (int_buckets_)
  {
    for (int st = 0; st <= max_street; ++st)
    {
      if (int_buckets_[st])
        delete[] int_buckets_[st];
    }
    delete[] int_buckets_;
  }
}
