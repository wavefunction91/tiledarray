# for each dependency track both current and previous id (the variable for the latter must contain PREVIOUS)
# to be able to auto-update them

# Boost explicitly downgraded to 1.59 from 1.68
set(TA_TRACKED_BOOST_VERSION 1.59)
set(TA_TRACKED_BOOST_PREVIOUS_VERSION 1.68)
set(TA_INSTALL_BOOST_VERSION 1.70.0)
set(TA_INSTALL_BOOST_PREVIOUS_VERSION 1.70.0)
set(TA_INSTALL_BOOST_URL_HASH 882b48708d211a5f48e60b0124cf5863c1534cd544ecd0664bb534a4b5d506e9)
set(TA_INSTALL_BOOST_PREVIOUS_URL_HASH 882b48708d211a5f48e60b0124cf5863c1534cd544ecd0664bb534a4b5d506e9)

# N.B. may need to update INSTALL.md manually with the CUDA-specific version
set(TA_TRACKED_EIGEN_VERSION 3.3)
set(TA_TRACKED_EIGEN_PREVIOUS_VERSION 3.3)
set(TA_INSTALL_EIGEN_VERSION 3.3.7)
set(TA_INSTALL_EIGEN_PREVIOUS_VERSION 3.3.7)
set(TA_INSTALL_EIGEN_URL_HASH b9e98a200d2455f06db9c661c5610496)
set(TA_INSTALL_EIGEN_PREVIOUS_URL_HASH b9e98a200d2455f06db9c661c5610496)

set(TA_TRACKED_MADNESS_TAG a3f3dce8c9d81262cf9fd7b29f97fcdafc7372a5)
set(TA_TRACKED_MADNESS_PREVIOUS_TAG b22ee85059e6ccc9a6e803ba0550652ece8d9df1)
set(TA_TRACKED_MADNESS_VERSION 0.10.1)
set(TA_TRACKED_MADNESS_PREVIOUS_VERSION 0.10.1)

set(TA_TRACKED_BTAS_TAG bbb11894802d7e2f89182a2e7fce9aed1078f851)
set(TA_TRACKED_BTAS_PREVIOUS_TAG 8752fb97fc68d1982c48283aba4b03c744389ba8)

set(TA_TRACKED_CUTT_TAG 0e8685bf82910bc7435835f846e88f1b39f47f09)
set(TA_TRACKED_CUTT_PREVIOUS_TAG 592198b93c93b7ca79e7900b9a9f2e79f9dafec3)

set(TA_TRACKED_UMPIRE_TAG f04abd1dd038c84262915a493d8f78576bb80fd0)
set(TA_TRACKED_UMPIRE_PREVIOUS_TAG f04abd1dd038c84262915a493d8f78576bb80fd0)

#set(TA_TRACKED_BLACSPP_TAG 20cfd414c5b719be1c958f4a2d57abef06df83b6 )
#set(TA_TRACKED_BLACSPP_PREVIOUS_TAG     da4ada57e578cf944325a7152164306742551596 )

set(TA_TRACKED_SCALAPACKPP_TAG 04a018cf24a74bd7d4cec66967c4bec850d8175c )
set(TA_TRACKED_SCALAPACKPP_PREVIOUS_TAG 04a018cf24a74bd7d4cec66967c4bec850d8175c )

set(TA_TRACKED_RANGEV3_TAG 2e0591c57fce2aca6073ad6e4fdc50d841827864)
set(TA_TRACKED_RANGEV3_PREVIOUS_TAG dbdaa247a25a0daa24c68f1286a5693c72ea0006)
