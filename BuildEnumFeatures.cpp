#include <stdio.h>
#include <stdlib.h>

#include "GenBDTree.h"
#include "constants.h"
#include "DiskFiles.h"
#include "string.h"
#include "GameInfo.h"
#include "HSTree.h"
#include "FileOperation.h"
#include "Enum.h"

using namespace std;

int main(int argc, char *argv[])
{
  DiskFiles::Init();
  GameInfo::Initialize();

  HSTree::Create();
  GenBDTree::Create();

  unsigned int street = 2;
  string features_name = "hs";

  int num_percentiles = 3;  

  double *percentiles = new double[num_percentiles]; 

  if(2 == street)
  {
    percentiles[0] = 0.4;
    percentiles[1] = 0.5;
    percentiles[2] = 0.6;
  }  

  int *pct_vals = ComputeRollout(street, percentiles, num_percentiles, 1.0, false);

  unsigned int num_boards = GenBDTree::NumBoards(street);
  unsigned int num_hole_card_pairs = GameInfo::NumHoleCardPairs(street);
  unsigned int num_hands = num_boards * num_hole_card_pairs;
  fprintf(stderr, "%u hands\n", num_hands);

  char buf[500];
  sprintf(buf, "%s/features.%s.%u.%s.%u", DiskFiles::GetstaticDirPath(), GameInfo::GameName().c_str(),
          GameInfo::NumRanks(), features_name.c_str(), street);
  Writer writer(buf);
  writer.WriteInt(num_percentiles);

  for (unsigned int h = 0; h < num_hands; ++h)
  {
    for (int p = 0; p < num_percentiles; ++p)
    {
      writer.WriteInt(pct_vals[h * num_percentiles + p]);
    }
  }

  ////generate the river
  street = 3;
  if(percentiles)
  {
    delete[] percentiles;
    percentiles = 0;
  }

  percentiles = new double;  
  *percentiles = 0.5;
  num_percentiles = 1;

  if(pct_vals)
  {
    delete[] pct_vals;
    pct_vals = 0;
  }

  pct_vals = ComputeRollout(street, percentiles, num_percentiles, 1.0, false);

  num_boards = GenBDTree::NumBoards(street);
  num_hole_card_pairs = GameInfo::NumHoleCardPairs(street);
  num_hands = num_boards * num_hole_card_pairs;
  fprintf(stderr, "%u hands\n", num_hands);

  memset(buf, 0, 500 * sizeof(char));
  sprintf(buf, "%s/features.%s.%u.%s.%u", DiskFiles::GetstaticDirPath(), GameInfo::GameName().c_str(),
          GameInfo::NumRanks(), features_name.c_str(), street);
  Writer writerT(buf);
  writerT.WriteInt(num_percentiles);

  for (unsigned int h = 0; h < num_hands; ++h)
  {
    for (int p = 0; p < num_percentiles; ++p)
    {
      writerT.WriteInt(pct_vals[h * num_percentiles + p]);
    }
  }


}
