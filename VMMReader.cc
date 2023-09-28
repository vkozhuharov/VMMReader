#include "PCAPNGReader.hh"
#include "SRSData.hh"
#include "Analyser.hh"
#include <iostream>
#include <string>
#include <chrono>


const int BUFSIZE = 100000;

int main( int argc, char * argv[] )
{
  for ( int a = 1; a < argc; ++a )
  {
    auto start = std::chrono::high_resolution_clock::now();
    
    char buf[BUFSIZE];
    std::string filename = argv[a];
    PCAPNGReader *PCAPStream = new PCAPNGReader( filename );
    SRSData *srs = new SRSData;
    Analyser *ana = new Analyser();
    
    if( PCAPStream->open() == 0 )
      std::cout << argv[a] << " opened successfully." << std::endl;
  
    else
    {
      std::cout << "Cannot open file." << std::endl;
      exit( 0 );
    }

    // ana->ClearTestFile();
    // ana->clearTreeFile();
    
    int dataSize = 0;
    int count = 1;
    int batchCount = 1;
    float ptimetot = 0;
    float timetot = 0;
    
    while( ( dataSize = PCAPStream->read( buf, BUFSIZE ) ) != -1 )
    {
      if( dataSize != 8968 ) continue;
      
      srs->setDataBuffer( buf, dataSize );
      int res = srs->decode();
      if ( res == -1 ) continue;
      
      auto pstart = std::chrono::high_resolution_clock::now();
      ana->process( srs, batchCount );
      batchCount++;
      auto pstop = std::chrono::high_resolution_clock::now();
      
      auto ptime = std::chrono::duration_cast< std::chrono::nanoseconds >( pstop - pstart ).count();
      timetot += ptime;

      if( count % 1000 == 0 )
      {
	std::cout << "\r" << std::string( 100, ' ' ) << std::flush;
	std::cout << "\r" << "Packets: " << count << ". Average packet analysis time: " << ( timetot/count ) / 1e3 << " us." << std::flush;
      }
      count++;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast< std::chrono::seconds >( stop - start ).count();
    std::cout << std::endl << "Final packet: " << count << ". Full analysis time: " << time << " s." << std::endl;

    ana->ogGrapher();
    ana->graphCutter();
    ana->finalFitter();
    
    if( srs ) delete srs;
    if( PCAPStream ) delete PCAPStream;


    PCAPNGReader *PCAPStream2 = new PCAPNGReader( filename );
    SRSData *srs2 = new SRSData;

    if( PCAPStream2->open() == 0 )
      std::cout << argv[a] << " reopened successfully." << std::endl;

    else
    {
      std::cout << "Cannot reopen file." << std::endl;
      exit( 0 );
    }

    char buf2[BUFSIZE];
    int dataSize2 = 0;
    int calCount = 1;
    double calTimeTot = 0;
    std::cout << std::endl;

    while( ( dataSize2 = PCAPStream2->read( buf2, BUFSIZE ) ) != -1 )
    {
      if( dataSize2 != 8968 ) continue;
      
      srs2->setDataBuffer( buf2, dataSize2 );
      int res2 = srs2->decode();
      if ( res2 == -1 ) continue;

      auto calstart = std::chrono::high_resolution_clock::now();
      ana->calibrator2( srs2 );
      auto calstop = std::chrono::high_resolution_clock::now();

      auto caltime = std::chrono::duration_cast< std::chrono::nanoseconds >( calstop - calstart ).count();
      calTimeTot += caltime;

      if( calCount % 1000 == 0 )
      {
	std::cout << "\r" << std::string( 100, ' ' ) << std::flush;
	std::cout << "\r" << "Packets: " << calCount << ". Average packet calibration time: " << ( calTimeTot/calCount ) / 1e3 << " us." << std::flush;
      }
      calCount++;
    }
    
    // PCAPStream->printStats();
    if( ana ) delete ana;
    if( srs2 ) delete srs;
    if( PCAPStream2 ) delete PCAPStream;

    auto rtstop = std::chrono::high_resolution_clock::now();
    auto rtime = std::chrono::duration_cast< std::chrono::seconds >( rtstop - start ).count();

    std::cout << "Done. Total runtime: " << rtime << " s." << std::endl;
  }
}
