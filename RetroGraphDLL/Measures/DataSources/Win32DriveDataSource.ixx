export module RG.Measures.DataSources:Win32DriveDataSource;

import :IDriveDataSource;

namespace rg {

export class Win32DriveDataSource : public IDriveDataSource {
public:
    Win32DriveDataSource() = default;
    ~Win32DriveDataSource() = default;

    DriveData getDriveData() const override;
};

} // namespace rg
