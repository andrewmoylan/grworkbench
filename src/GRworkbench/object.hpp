#pragma once

namespace grwb
{
  class view;

  class object
  {
  public:
		explicit object(view& v)
			: view_(v)
		{
		}

    virtual ~object()
		{
		}

    virtual void invalidate() = 0;
    virtual void draw() const = 0;

		view& view_;
  private:
    object& operator=(const object&);
  };
}
