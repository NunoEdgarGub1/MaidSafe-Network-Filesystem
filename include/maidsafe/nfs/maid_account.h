/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#ifndef MAIDSAFE_NFS_MAID_ACCOUNT_H_
#define MAIDSAFE_NFS_MAID_ACCOUNT_H_

#include <vector>

#include "maidsafe/common/types.h"

namespace maidsafe {

namespace nfs {

class PmidRegistration {
 public:
  PmidRegistration(Identity maid_id_in, Identity pmid_id_in, bool register_in,
                   NonEmptyString maid_signature_in, NonEmptyString pmid_signature_in)
    : maid_id_(maid_id_in),
      pmid_id_(pmid_id_in),
      register_(register_in),
      maid_signature_(maid_signature_in),
      pmid_signature_(pmid_signature_in) {}
  explicit PmidRegistration(const NonEmptyString& serialised_pmidregistration)
    : maid_id_(),
      pmid_id_(),
      register_(false),
      maid_signature_(NonEmptyString()),
      pmid_signature_(NonEmptyString()) {
    Parse(serialised_pmidregistration);
  }

  void Parse(const NonEmptyString& serialised_pmidregistration);
  NonEmptyString Serialise();
  Identity maid_id() { return maid_id_; }
  Identity pmid_id() { return pmid_id_; }

 private:
  Identity maid_id_;
  Identity pmid_id_;
  bool register_;
  NonEmptyString maid_signature_;
  NonEmptyString pmid_signature_;
};

class PmidSize {
 public:
  explicit PmidSize(Identity pmid_id_in)
    : pmid_id(pmid_id_in),
      num_data_elements(0),
      total_size(0),
      lost_size(0),
      lost_number_of_elements(0) {}
  explicit PmidSize(const NonEmptyString& serialised_pmidsize)
    : pmid_id(),
      num_data_elements(0),
      total_size(0),
      lost_size(0),
      lost_number_of_elements(0) {
    Parse(serialised_pmidsize);
  }

  void Parse(const NonEmptyString& serialised_pmidsize);
  NonEmptyString Serialise();

 private:
  Identity pmid_id;
  int32_t num_data_elements;
  int64_t total_size;
  int64_t lost_size;
  int64_t lost_number_of_elements;
};

class PmidTotal {
 public:
  PmidTotal(PmidRegistration registration_in, PmidSize pmid_size_in)
    : registration(registration_in), pmid_size(pmid_size_in) {}

  NonEmptyString Serialise();
  bool IsRecordOf(const Identity& pmid_id) {
    return pmid_id == registration.pmid_id();
  }
  Identity pmid_id() { return registration.pmid_id(); }

 private:
  PmidRegistration registration;
  PmidSize pmid_size;
};

class DataElement {
 public:
  DataElement() : data_id_(), data_size(0) {}

  DataElement(Identity data_id_in, int32_t data_size_in)
    : data_id_(data_id_in), data_size(data_size_in) {}

  NonEmptyString Serialise();
  Identity data_id() { return data_id_; }

 private:
  Identity data_id_;
  int32_t data_size;
};

class MaidAccount {
 public:
  MaidAccount()
    : maid_id_(),
      pmid_totals(),
      data_elements() {}

  explicit MaidAccount(Identity maid_id_in)
    : maid_id_(maid_id_in),
      pmid_totals(),
      data_elements() {}

  explicit MaidAccount(const NonEmptyString& serialised_maidaccount)
    : maid_id_(),
      pmid_totals(),
      data_elements() {
    Parse(serialised_maidaccount);
  }

  void Parse(const NonEmptyString& serialised_maidaccount);
  NonEmptyString Serialise();
  void PushPmidTotal(PmidTotal pmid_total) {
    pmid_totals.push_back(pmid_total);
  }
  void RemovePmidTotal(Identity pmid_id) {
    for (auto itr = pmid_totals.begin(); itr != pmid_totals.end(); ++itr) {
      if ((*itr).IsRecordOf(pmid_id)) {
        pmid_totals.erase(itr);
        return;
      }
    }
  }
  void UpdatePmidTotal(PmidTotal pmid_total) {
    RemovePmidTotal(pmid_total.pmid_id());
    PushPmidTotal(pmid_total);
  }
  void PushDataElement(DataElement data_element) {
    data_elements.push_back(data_element);
  }
  void RemoveDataElement(Identity data_id) {
    for (auto itr = data_elements.begin(); itr != data_elements.end(); ++itr) {
      if ((*itr).data_id() == data_id) {
        data_elements.erase(itr);
        return;
      }
    }
  }
  void UpdateDataElement(DataElement data_element) {
    RemoveDataElement(data_element.data_id());
    PushDataElement(data_element);
  }
  Identity maid_id() { return maid_id_; }

 private:
  Identity maid_id_;
  std::vector<PmidTotal> pmid_totals;
  std::vector<DataElement> data_elements;
};

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MAID_ACCOUNT_H_