# CyberSouls Git Upload Script
# Bu script'i çalıştırarak projenizi GitHub'a yükleyebilirsiniz

Write-Host "CyberSouls GitHub Upload Script" -ForegroundColor Cyan
Write-Host "================================" -ForegroundColor Cyan

# Mevcut dizini kontrol et
$currentDir = Get-Location
if ($currentDir.Path -ne "D:\Unreal Projects\cybersouls") {
    Write-Host "Proje dizinine geçiliyor..." -ForegroundColor Yellow
    Set-Location "D:\Unreal Projects\cybersouls"
}

# Git durumunu kontrol et
Write-Host "`nGit durumu kontrol ediliyor..." -ForegroundColor Yellow
git status --short

# Kullanıcıya devam etmek isteyip istemediğini sor
$confirm = Read-Host "`nDevam etmek istiyor musunuz? (E/H)"
if ($confirm -ne "E" -and $confirm -ne "e") {
    Write-Host "İşlem iptal edildi." -ForegroundColor Red
    exit
}

# Git LFS'i kontrol et
Write-Host "`nGit LFS kontrol ediliyor..." -ForegroundColor Yellow
git lfs version
if ($LASTEXITCODE -ne 0) {
    Write-Host "Git LFS yüklü değil! Lütfen önce Git LFS'i yükleyin." -ForegroundColor Red
    exit
}

# Değişiklikleri ekle
Write-Host "`nDeğişiklikler ekleniyor..." -ForegroundColor Yellow
git add .

# Durum özeti
Write-Host "`nEklenecek dosyalar:" -ForegroundColor Yellow
git status --short | Select-Object -First 20
$fileCount = (git status --short | Measure-Object -Line).Lines
Write-Host "Toplam: $fileCount dosya" -ForegroundColor Green

# LFS dosyalarını göster
Write-Host "`nLFS ile takip edilen dosyalar:" -ForegroundColor Yellow
git lfs status

# Commit mesajı al
$commitMessage = Read-Host "`nCommit mesajı girin"
if ([string]::IsNullOrWhiteSpace($commitMessage)) {
    $commitMessage = "Update CyberSouls project files"
}

# Commit yap
Write-Host "`nCommit yapılıyor..." -ForegroundColor Yellow
git commit -m "$commitMessage"

# Push yap
Write-Host "`nGitHub'a yükleniyor..." -ForegroundColor Yellow
Write-Host "Bu işlem büyük dosyalar için zaman alabilir..." -ForegroundColor Gray
git push origin master

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nProje başarıyla GitHub'a yüklendi!" -ForegroundColor Green
    Write-Host "Repository: https://github.com/BYGGOLDENSTONE/CyberSouls" -ForegroundColor Cyan
} else {
    Write-Host "`nHata oluştu! Lütfen yukarıdaki hata mesajını kontrol edin." -ForegroundColor Red
}

# Bekle
Write-Host "`nKapatmak için bir tuşa basın..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")