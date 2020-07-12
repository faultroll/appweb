class Appweb < Formula
<<<<<<< HEAD
  homepage "https://embedthis.com/appweb"
=======
  homepage "https://www.embedthis.com/appweb"
>>>>>>> local
  url "https://github.com/embedthis/appweb/archive/v6.2.1.tar.gz"
  sha1 "2367c53bd181f29270ecf0321b8344b2ed49fa9d"

  # version "6.2.1"

  depends_on 'makeme'

  def install
    ENV.deparallelize
    ENV.prepend_path "PATH", "/usr/local/bin"
    system "me", "configure", "--release", "--prefix", "app=#{HOMEBREW_PREFIX}/Cellar/#{name}"
    system "me", "compile", "install"
  end

  test do
    system "#{bin}/appweb"
  end
end
